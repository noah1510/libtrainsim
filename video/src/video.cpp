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
    
    try{
        nextFrame.wait();
    }catch(...){}
    
    free(frame_data);
    
    std::cout << "video manager for window '" << windowName << "' was destroyed" << std::endl;
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

void libtrainsim::Video::videoManager::createWindow ( const std::string& windowName ) {
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
        
        YUVShader = std::make_shared<libtrainsim::Video::Shader>("data/production_data/shaders/YUV.vert","data/production_data/shaders/YUV.frag");
        YUVShader->setUniform("videoTex", 0);
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not create shader"));
    }

    currentWindowName = windowName;
    
    /*float vertices[] = {
    // position    // texture coords
    1.0f,  1.0f,   1.0f, 0.0f, // top right
    1.0f, -1.0f,   1.0f, 1.0f, // bottom right
    -1.0f, -1.0f,   0.0f, 1.0f, // bottom left
    -1.0f,  1.0f,   0.0f, 0.0f  // top left 
    };*/
    
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

    //init the output framebuffer and its texture
    imguiHandler::initFramebuffer(outputFBO,outputTexture,frameBufferWidth,frameBufferHeight);
    
    //reset the last frame and receive the first frame from the decoder
    /*lastFrame.reset();
    try{
        lastFrame = decode.getNextFrame();
        decode.scaleFrame(lastFrame, frame_data);
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not retrieve next frame"));
    }*/
    
    //video_reader_read_frame(state);
    //frame_data.resize(state.width*state.height*8);
    //video_reader_copy_to_buffer(state, frame_data.data());
    //frame_data.shrink_to_fit();
    constexpr int ALIGNMENT = 128;
    const auto [frame_width,frame_height] = decode->getDimensions();
    if (posix_memalign((void**)&frame_data, ALIGNMENT, frame_width * frame_height * 4) != 0) {
        throw std::runtime_error("Couldn't allocate frame buffer\n");
    }
    
    decode->copyToBuffer(frame_data);
    
    //init the YUV textures so that they can be written into
    initYUVTexture();
    
    //refresh the window to display stuff
    updateYUVImage();
    
    windowFullyCreated = true;
}

void libtrainsim::Video::videoManager::initYUVTexture() {
    
    //texture Y
    
    //create the texture unit
    glGenTextures(1, &textureVideo);
    glBindTexture(GL_TEXTURE_2D, textureVideo); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);// set texture wrapping to GL_REPEAT (default wrapping method)
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    //float color[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
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
                    decode->seekFrame(nextF);   return false;
                
                }else{
                    bool working = true;
                    while(currF < nextF && working){
                        decode->readNextFrame();
                        currF++;
                    }
                }
            }catch(const std::exception& e){
                libtrainsim::core::Helper::print_exception(e);
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

void libtrainsim::Video::videoManager::updateYUVImage() {
    
    imguiHandler::loadFramebuffer(outputFBO, frameBufferWidth, frameBufferHeight);
    
    YUVShader->use();
    
    float camMult = 1.1;
    auto orth = glm::ortho(
        -16.0f, 
        camMult * 9.0f * frameBufferWidth / frameBufferHeight,
        -9.0f,
        camMult * 16.0f * frameBufferHeight / frameBufferWidth,
        -10.0f,
        10.0f
    );
    YUVShader->setUniform("transform", orth);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureVideo);
    
    auto [w,h] = decode->getDimensions();
    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_RGBA, 
        w, 
        h, 
        0,
        GL_RGBA, 
        GL_UNSIGNED_BYTE, 
        //frame_data.data()
        frame_data
    );
    
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
        auto status = nextFrame.wait_for(1ms);
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
    ImGui::Begin(currentWindowName.c_str());
    
        //update the output size
        frameBufferHeight = ImGui::GetWindowHeight();
        frameBufferWidth = ImGui::GetWindowWidth();
        
        //render into output texture
        updateYUVImage();
        
        //draw the output texture
        ImGui::Image((void*)(intptr_t)outputTexture, ImVec2(frameBufferWidth, frameBufferHeight));
        //ImGui::Image(frame_data.data(), ImVec2(decode.getWidth(), decode.getHeight()));
        
        //show a tooltip when the window is hovered
        if(ImGui::IsItemHovered()){
            ImGui::BeginTooltip();
            ImGui::Text("pointer = %u", outputTexture);
            ImGui::Text("size = %lu x %lu", frameBufferWidth, frameBufferHeight);
            ImGui::EndTooltip();
        }
    
    ImGui::End();

}






  
