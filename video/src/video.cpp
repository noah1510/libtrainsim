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
    std::cout << "locking video manager to prevent draw calls while destroying" << std::endl;
    std::scoped_lock<std::shared_mutex> lock{videoMutex};
    windowFullyCreated = false;
    
    try{
        std::cout << "waiting for queued frame to render" << std::endl;
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
        imguiHandler::loadShaders(shaderLocation);
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not load imgui shader parts"));
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
    
    /*
    //This is the sample code to add an additional texture to the render code to be displayed on top of the background
    try{
        auto snow_tex = std::make_shared<texture>(shaderLocation / "../snowflake_textures/snowflake-0.tif");
        addTexture(snow_tex);
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not create the test snowflake texture"));
    }
    */

    currentWindowName = windowName;
    
    //reset the last frame and receive the first frame from the decoder
    decode->copyToBuffer(frame_data[0]);
    
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
                    sakurajin::unit_system::base::time_si rendertime;
                    while(currF < nextF && working){
                        auto time = decode->readNextFrame();
                    
                        rendertime += time;
                        currF++;
                    }

                    renderTimeMutex.lock();
                    newRenderTimes.emplace_back(rendertime);
                    renderTimeMutex.unlock();   
                }
                
                
                frameBuffer_mutex.lock();
                uint64_t inactiveBuffer = frontBufferActive ? 1 : 0;
                frameBuffer_mutex.unlock();

                decode->copyToBuffer(frame_data[inactiveBuffer]);

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
    float camMult = 1;
    float halfScreenWidth = camMult * (16.0f/9.0f * h / w + 1.0f)/2.0f;
    auto orth = glm::ortho(
        -halfScreenWidth, 
        halfScreenWidth,
        -1.0f,
        1.0f,
        -10.0f,
        10.0f
    );
    displayShader->setUniform("transform", orth*rot);
    
    for(unsigned int i = 0; i < displayTextures.size(); i++){
        displayTextures[i]->bind(i);
    }
    
    imguiHandler::drawRect();
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
                videoMutex.unlock();
                throw std::runtime_error("Error getting next frame");
            }
            
            frameBuffer_mutex.lock();

            frontBufferActive = !frontBufferActive;
            uint64_t activeBuffer = frontBufferActive ? 0 : 1;
            displayTextures[0]->updateImage(frame_data[activeBuffer], decode->getDimensions());

            frameBuffer_mutex.unlock();

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
        ImGui::SetNextWindowContentSize( decode->getDimensions() );
        
        ImVec2 initialPos {0,0};
        ImGui::SetNextWindowPos(initialPos);
        
        firstStart = false;
    }
    
    //actually start drawing the window
    ImGui::Begin(currentWindowName.c_str(), &isActive);
    
        //update the output size
        auto w = ImGui::GetWindowWidth();
        auto h = ImGui::GetWindowHeight();

        libtrainsim::Video::dimensions newSize = ImGui::GetContentRegionAvail();
        
        outputBuffer->resize(newSize);
        
        //render into output texture
        updateOutput();
        
        //draw the output texture
        outputBuffer->displayImGui();
        
        //show a tooltip when the window is hovered
        if(ImGui::IsItemHovered()){
            ImGui::BeginTooltip();
            ImGui::Text("size = %f x %f", w, h);
            ImGui::EndTooltip();
        }
    
    ImGui::End();

}

void libtrainsim::Video::videoManager::addTexture ( std::shared_ptr<texture> newTexture ) {
    if(displayTextures.size() == libtrainsim::Video::imguiHandler::getMaxTextureUnits()){
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


