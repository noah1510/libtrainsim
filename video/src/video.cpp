#include "video.hpp"

using namespace std::literals;

libtrainsim::Video::videoManager::videoManager(){
    try{
        imguiHandler::init();
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not create instance"));
    }
}

libtrainsim::Video::videoManager::~videoManager(){
    
    std::scoped_lock<std::shared_mutex> lock{videoMutex};
    windowFullyCreated = false;
    
    try{
        nextFrame.wait();
    }catch(...){}
    
    std::cout << "destroying video decoder" << std::endl;
    decode.reset();
    
    std::cout << "video manager for window '" << currentWindowName << "' was destroyed" << std::endl;
}

void libtrainsim::Video::videoManager::load(const std::filesystem::path& uri){
    try{
        decode = std::make_unique<libtrainsim::Video::videoReader>(uri);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not init video renderer"));
    }
}

const std::filesystem::path& libtrainsim::Video::videoManager::getFilePath() const{
    return decode->getLoadedFile();
}

void libtrainsim::Video::videoManager::createWindow ( const std::string& windowName, const std::filesystem::path& shaderLocation ) {
    if(currentWindowName != "" || windowFullyCreated){
        throw std::runtime_error("window alread exists");
    }
    
    if(decode == nullptr){
        throw std::runtime_error("render is not loaded correctly yet, state.uri is still empty");
    }
    
    if(decode->reachedEndOfFile()){
        throw std::runtime_error("renderer is already done. try creating the window with a new renderer.");
    }
    
    if(windowName == ""){
        throw std::invalid_argument("The window name may not be an empty string");
    }
    
    try{
        outputBuffer = std::make_shared<texture>("outBuffer"s);
        
        //init the output framebuffer and its texture
        outputBuffer->createFramebuffer(decode->getDimensions());
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create the output framebuffer"));
    }
    
    try{
        displayShader = std::make_shared<libtrainsim::Video::Shader>(shaderLocation/"display.vert", shaderLocation/"display.frag");
        
        displayShader->use();
        std::vector<int> units {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
        displayShader->setUniform("tex", units);
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not create shader"));
    }
    
    try{
        auto bg_tex = std::make_shared<texture>("background"s);
        addTexture(bg_tex);
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not init video image"));
    }

    currentWindowName = windowName;
    
    float vertices[] = {
        // positions    // texture coords
         16.0f,  9.0f,  1.0f, 1.0f, // top right
         16.0f, -9.0f,  1.0f, 0.0f, // bottom right
        -16.0f, -9.0f,  0.0f, 0.0f, // bottom left
        -16.0f,  9.0f,  0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    
    //create all of the vertex buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    //reset the last frame and receive the first frame from the decoder
    decode->copyToBuffer(frame_data);
    
    //refresh the window to display stuff
    updateOutput();
    
    windowFullyCreated = true;
}

double libtrainsim::Video::videoManager::getHeight() {
    return decode->getDimensions().second;
}

double libtrainsim::Video::videoManager::getWidth() {
    return decode->getDimensions().first;
}

void libtrainsim::Video::videoManager::gotoFrame ( uint64_t frame_num ) {
    
    std::scoped_lock<std::shared_mutex> lock{videoMutex};

    nextFrameToGet = frame_num > nextFrameToGet ? frame_num : nextFrameToGet;
    
    if(fetchingFrame){
        return;
    }
    
    if(decode->getFrameNumber() >= nextFrameToGet){
        return;
    }
    
    nextFrame = std::async(
        std::launch::async, 
        [this](){
            uint64_t nextF = nextFrameToGet;
            uint64_t currF = decode->getFrameNumber();
            uint64_t diff = nextF - currF;
            
            try{
                
                if(diff > 120){
                    auto time = decode->seekFrame(nextF);
                    
                    renderTimeMutex.lock();
                    newRenderTimes.emplace_back(time);
                    renderTimeMutex.unlock();
                
                }else{
                    bool working = true;
                    std::vector<sakurajin::unit_system::base::time_si> times;
                    while(currF < nextF && working){
                        auto time = decode->readNextFrame();
                    
                        times.emplace_back(time);
                        currF++;
                    }
                    
                    sakurajin::unit_system::base::time_si rendertime;
                    for(auto time: times){
                        rendertime += time;
                    }
                    renderTimeMutex.lock();
                    newRenderTimes.emplace_back(rendertime);
                    renderTimeMutex.unlock();
                    
                }
            }catch(const std::exception& e){
                libtrainsim::core::Helper::print_exception(e);
                return false;
            }
            
            return true;
        }
    );
    fetchingFrame = true;
}

bool libtrainsim::Video::videoManager::reachedEndOfFile() {
    return decode->reachedEndOfFile();
    //return decode.reachedEndOfFile();
}

void libtrainsim::Video::videoManager::updateOutput() {
    
    outputBuffer->loadFramebuffer();
    auto [w,h] = outputBuffer->getSize();
    
    displayShader->use();
    auto rot = glm::mat4{1.0f};
    rot = glm::scale(rot, {1.0f,-1.0f,1.0f});
    displayShader->setUniform("transform", outputBuffer->getProjection() *rot);
    
    glActiveTexture(GL_TEXTURE0);
    displayTextures[0]->updateImage(frame_data, decode->getDimensions());
    
    for(unsigned int i = 1; i < displayTextures.size(); i++){
        displayTextures[i]->bind(i);
    }
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void libtrainsim::Video::videoManager::refreshWindow() {
    if(!windowFullyCreated){
        return;
    }
    
    videoMutex.lock();
    
    if(fetchingFrame){
        auto status = nextFrame.wait_for(1ns);
        if(status == std::future_status::ready){
            if(!nextFrame.get()){
                std::cerr << "Error getting next frame" << std::endl;
            }
            try{
                decode->copyToBuffer(frame_data);
            }catch(...){
                std::throw_with_nested(std::runtime_error("Could not read current frame into buffer"));
            }
            fetchingFrame = false;
        }
    }
    
    videoMutex.unlock();
    
    //start fetching the next queued fram asap
    //if no higher frame number is queued then this call does nothing
    gotoFrame(0);

    //draw the render window
    
    //set size and pos on program start to initial values
    static bool firstStart = true;
    if(firstStart){
        auto [w,h] = decode->getDimensions();
        ImVec2 initialSize {static_cast<float>(w),static_cast<float> (h)};
        ImGui::SetNextWindowContentSize( initialSize );
        
        ImVec2 initialPos {0,0};
        ImGui::SetNextWindowPos(initialPos);
        
        firstStart = false;
    }
    
    //actually start drawing the window
    ImGui::Begin(currentWindowName.c_str(), &isActive);
        
        //render into output texture
        updateOutput();

        //get the image size
        libtrainsim::Video::dimensions newSize = ImGui::GetContentRegionAvail();
        auto dim = newSize.x()/newSize.y();
        
        //correct the new image size to keep the image ratio at 16:9
        if(dim > 16.0/9.0){
            newSize.x() = newSize.y() / 9.0 * 16.0;
        }else{
            newSize.y() = newSize.x() / 16.0 * 9.0;
        }
        
        //draw the output texture
        outputBuffer->displayImGui(newSize);
        
        //show a tooltip when the window is hovered
        /*if(ImGui::IsItemHovered()){
            ImGui::BeginTooltip();
            ImGui::Text("size = %f x %f", w, h);
            ImGui::EndTooltip();
        }*/
    
    ImGui::End();

}

void libtrainsim::Video::videoManager::addTexture ( std::shared_ptr<texture> newTexture ) {
    if(displayTextures.size() == 16){
        throw std::runtime_error("For now only 16 display textures are supported, remove one to add this one!");
    }
    
    auto texName = newTexture->getName();

    for(auto x:displayTextures){
        if(x->getName() == texName){
            throw std::invalid_argument("a texture with this name already exists");
        }
    }
    
    displayTextures.emplace_back(newTexture);
    displayShader->use();
    displayShader->setUniform("enabledUnits", displayTextures.size());
}

void libtrainsim::Video::videoManager::removeTexture ( const std::string& textureName ) {
    if(textureName == "background"){
        throw std::invalid_argument("the background texture cannot be removed");
    }
    
    for(auto i = displayTextures.begin(); i < displayTextures.end(); i++){
        if((*i)->getName() == textureName){
            displayTextures.erase(i);

            displayShader->use();
            displayShader->setUniform("enabledUnits", displayTextures.size());
            return;
        }
    }
    
    throw std::invalid_argument("a texture with the name '" + textureName + "' does not exist");
}

std::optional<std::vector<sakurajin::unit_system::base::time_si>> libtrainsim::Video::videoManager::getNewRendertimes() {
    renderTimeMutex.lock();
    auto times = newRenderTimes;
    newRenderTimes.clear();
    renderTimeMutex.unlock();
    
    if(times.size() > 0){
        return std::make_optional(times);
    }else{
        return {};
    }
}


