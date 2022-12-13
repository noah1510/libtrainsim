#include "imguiHandler.hpp"
#include "texture.hpp"
#include "shader.hpp"

using namespace std::literals;

void libtrainsim::Video::styleSettings::displayContent() {
    imguiHandler& handle = imguiHandler::getInstance();
    float clearCol[3] = {handle.clear_color.x,handle.clear_color.y,handle.clear_color.z};
    
    ImGui::ShowStyleEditor(&ImGui::GetStyle());
        
    //add an 'empty' line between the options
    ImGui::Text(" ");
    
    //display the color changer for the clear color
    ImGui::TableNextColumn();
    ImGui::ColorPicker3("clear color picker", clearCol);
    if(ImGui::IsItemHovered()){
        ImGui::SetTooltip("change the background color of the background window");
    }

    handle.clear_color = ImVec4{clearCol[0],clearCol[1],clearCol[2],1.0};
}

libtrainsim::Video::styleSettings::styleSettings() : tabPage("style"){}

void libtrainsim::Video::basicSettings::displayContent() {
    imguiHandler& handle = imguiHandler::getInstance();
    
    ImGui::BeginTable("basic Settings table", 1, (ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_SizingStretchProp));
        //display the style changer with its tooltip
        ImGui::TableNextColumn();
        ImGui::ShowStyleSelector("active imgui style");
        if(ImGui::IsItemHovered()){
            ImGui::SetTooltip("change the style of the windows");
        }
        
        //add an 'empty' line between the options
        ImGui::TableNextColumn();
        ImGui::Text(" ");
        
        //display the default FBO size selection
        ImGui::TableNextColumn();
        int sizeY = handle.defaultFBOSize.y();
        
        static size_t fboSizeIndex = 1;
        if(ImGui::BeginCombo("Select default FBO size", FBOsizeOptions.at(fboSizeIndex).first.c_str() )){
            for(size_t i = 0; i < FBOsizeOptions.size();i++){
                if(ImGui::Selectable(FBOsizeOptions.at(i).first.c_str(), fboSizeIndex == i)){
                    fboSizeIndex = i;
                }
            }
            
            ImGui::EndCombo();
        }
        
        sizeY = FBOsizeOptions.at(fboSizeIndex).second;
        handle.defaultFBOSize.x() = sizeY*16/9;
        handle.defaultFBOSize.y() = sizeY;
        
    ImGui::EndTable();
}

libtrainsim::Video::basicSettings::basicSettings() : tabPage("basic"), FBOsizeOptions{{
    {"720p", 720},
    {"1080p", 1080},
    {"1440p", 1440},
    {"2160p", 2160}
}}{}

libtrainsim::Video::settingsWindow::settingsWindow() : window("Settings Window") {
    
    flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
    showWindow = false;
    
    addSettingsTab(std::make_shared<basicSettings>());
    addSettingsTab(std::make_shared<styleSettings>());
    
}

void libtrainsim::Video::settingsWindow::drawContent() {
    if(ImGui::BeginTabBar("settings tabs")){
        for(auto& tab:settingsTabs){(*tab)();}
        ImGui::EndTabBar();
    }
}


void libtrainsim::Video::settingsWindow::addSettingsTab ( std::shared_ptr<tabPage> newTab ) {
    for(auto& tab:settingsTabs){
        if(tab->getName() == newTab->getName()){
            throw std::invalid_argument{"a tab with this name already exists in the settings"};
        }
    }
    settingsTabs.emplace_back(newTab);

}

void libtrainsim::Video::settingsWindow::removeSettingsTab ( std::string_view tabName ) {
    auto i = settingsTabs.begin();
    i+=2;
    while(i != settingsTabs.end()){
        if((*i)->getName() == tabName){
            settingsTabs.erase(i);
            return;
        }
        i++;
    }
}
    


libtrainsim::Video::imguiHandler::imguiHandler(std::string windowName){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        throw std::runtime_error(SDL_GetError());
    }
    
    if( IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF | IMG_INIT_WEBP) == 0){
        throw std::runtime_error( "error initializing sdl_image"s + IMG_GetError() );
    }

    // GL 4.6 + GLSL 460
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_SetHint(SDL_HINT_RENDER_OPENGL_SHADERS, "1");
    
    //set all of the opengl ontext flags
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MAXIMIZED);
    sdl_window = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    gl_context = SDL_GL_CreateContext(sdl_window);
    SDL_GL_MakeCurrent(sdl_window, gl_context);
    
    #ifdef ENABLE_VSYNC
        SDL_GL_SetSwapInterval(1); // Enable vsync
    #else
        SDL_GL_SetSwapInterval(0); // Disable vsync
    #endif

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;
    
    // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    // Enable Docking Support
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //enable multi viewport support
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui::GetStyle().WindowTitleAlign.y = 0.5;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(sdl_window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
    
    // Load GL extensions using glad
    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        throw std::runtime_error("Could not create OpenGL context");
    }
    
    std::cout << "OpenGL version loaded: " << GLVersion.major << "." << GLVersion.minor << std::endl;
    std::cout << "OpenGL driver vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL driver renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    mainThreadID = std::this_thread::get_id();
    
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    std::cout << "maxTextureUnits: " << maxTextureUnits << std::endl;
    
    for(auto& tex:darkSteps){
        tex = nullptr;
    }
    
    settingsWin = std::make_unique<settingsWindow>();
}

libtrainsim::Video::imguiHandler::~imguiHandler() {
    if(shaderLoaded){
        copyShader.reset();
        displacement0.reset();
        for(auto& tex: darkSteps){
            tex.reset();
        }
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
    SDL_DestroyWindow(sdl_window);
    IMG_Quit();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void libtrainsim::Video::imguiHandler::init_impl() {}

void libtrainsim::Video::imguiHandler::startRender_impl() {
    IOLock.lock();
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        ImGui_ImplSDL2_ProcessEvent(&event);
        if(event.type == SDL_QUIT){
            teminateProgram = true;
        }
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    //control if the settings window is shown
    bool displayImGUiSettings = settingsWin->isVisible();
    if(!displayImGUiSettings){
        ImGui::BeginMainMenuBar();
            ImGui::MenuItem("Settings", NULL, &displayImGUiSettings);
        ImGui::EndMainMenuBar();
    }
    
    if(settingsWin->isVisible() != displayImGUiSettings){
        settingsWin->show();
    }
    settingsWin->draw();
    
}

void libtrainsim::Video::imguiHandler::endRender_impl() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    setViewport(io.DisplaySize);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(sdl_window);
    
    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable){
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
    
    IOLock.unlock();
}

void libtrainsim::Video::imguiHandler::initFramebuffer_impl ( unsigned int& FBO, unsigned int& texture, dimensions dims ) {
    warnOffThread();
    
    auto width = static_cast<unsigned int>(dims.x());
    auto height = static_cast<unsigned int>(dims.y());
    
    //create the framebuffer for the output image
    glGenFramebuffers(1, &FBO);
    
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    
    if(texture == 0){
        glGenTextures(1, &texture);
    }
    
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    
    unsigned int DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE){
        std::stringstream ss;
        ss << "Could not create output framebuffer. Error: " << decodeGLFramebufferStatus(status);
        throw std::runtime_error(ss.str());
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void libtrainsim::Video::imguiHandler::loadFramebuffer_impl ( unsigned int buf, dimensions dims, glm::vec4 clearColor ) {
    warnOffThread();
    
    glBindFramebuffer(GL_FRAMEBUFFER, buf);
    setViewport(dims);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
        
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendEquation(GL_MAX);
    
    glClearColor(clearColor.r,clearColor.g,clearColor.b,clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void libtrainsim::Video::imguiHandler::setViewport ( const libtrainsim::Video::dimensions& viewportSize ) {
    warnOffThread();
    if(forceViewportUpdate || viewportSize != lastViewportSize){
        forceViewportUpdate = false;
        lastViewportSize = viewportSize;
        
        auto width = static_cast<unsigned int>(lastViewportSize.x());
        auto height = static_cast<unsigned int>(lastViewportSize.y());
        glViewport(0, 0, width, height);
    }
}


void libtrainsim::Video::imguiHandler::updateRenderThread_impl() {
    std::scoped_lock lock{IOLock};
    forceViewportUpdate = true;
    SDL_GL_MakeCurrent(sdl_window, gl_context);
    mainThreadID = std::this_thread::get_id();
}

void libtrainsim::Video::imguiHandler::copy_impl ( std::shared_ptr<libtrainsim::Video::texture> src, std::shared_ptr<libtrainsim::Video::texture> dest, bool loadTexture, glm::mat4 transformation ) {
    
    //thorw an error if shader are not loaded yet
    if(!shaderLoaded){
        throw std::runtime_error("load shader before using the shader parts");
    }
    
    if(src == nullptr || dest == nullptr){
        throw std::invalid_argument("nullptr not allowed for copy operation");
    }
    
    if(!dest->hasFramebuffer()){
        throw std::invalid_argument("destination texture has no attached framebuffer");
    }
    dest->loadFramebuffer();
    
    copyShader->use();
    auto orth = glm::ortho(
        -1.0f, 
        1.0f,
        -1.0f,
        1.0f,
        -10.0f,
        10.0f
    );
    copyShader->setUniform("transform", orth * transformation);
    
    if(loadTexture){
        src->bind(15);
        
        copyShader->setUniform("sourceImage", 15);
    }
    
    drawRect();
    
    //reset all of the buffers
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void libtrainsim::Video::imguiHandler::loadShaders_impl ( const std::filesystem::path& shaderLocation, const std::filesystem::path& textureLocation ) {
    
    //do not reload if shader are already loaded
    if(shaderLoaded){
        return;
    }
    
    //---------------init Shader---------------
    //load the copy shader
    try{
        copyShader = std::make_shared<libtrainsim::Video::Shader>(shaderLocation/"copy.vert",shaderLocation/"copy.frag");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create copy shader"));
    }
    //load the draw shader
    try{
        drawShader = std::make_shared<libtrainsim::Video::Shader>(shaderLocation/"copy.vert",shaderLocation/"draw.frag");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create draw shader"));
    }
    
    //---------------load textures---------------
    try{
        displacement0 = std::make_shared<libtrainsim::Video::texture>(textureLocation/"displacement-0.tif");
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not init displacement-0 texture"));
    }
    
    //---------------init vertex buffers---------------
    float vertices[] = {
        // position           // texture coords
         1.0f,  1.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    
    //create all of the blit buffers
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
    
    shaderLoaded = true;
}

void libtrainsim::Video::imguiHandler::bindVAO_impl() {
    
    //thorw an error if shader are not loaded yet
    if(!shaderLoaded){
        throw std::runtime_error("load shader before using the shader parts");
    }
    
    glBindVertexArray(VAO);
}

void libtrainsim::Video::imguiHandler::drawRect_impl() {
    //thorw an error if shader are not loaded yet
    if(!shaderLoaded){
        throw std::runtime_error("load shader before using the shader parts");
    }
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void libtrainsim::Video::imguiHandler::drawColor_impl ( std::shared_ptr<texture> dest, glm::vec4 color ) {
    //thorw an error if shader are not loaded yet
    if(!shaderLoaded){
        throw std::runtime_error("load shader before using the shader parts");
    }
    
    if(dest == nullptr){
        throw std::invalid_argument("nullptr not allowed for copy operation");
    }
    
    if(!dest->hasFramebuffer()){
        throw std::invalid_argument("destination texture has no attached framebuffer");
    }
    dest->loadFramebuffer();
    
    drawShader->use();
    auto orth = glm::ortho(
        -1.0f, 
        1.0f,
        -1.0f,
        1.0f,
        -10.0f,
        10.0f
    );
    drawShader->setUniform("transform", orth);
    drawShader->setUniform("color", color);
    
    drawRect();
    
    //reset all of the buffers
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


std::shared_ptr<libtrainsim::Video::texture> libtrainsim::Video::imguiHandler::getDarkenTexture_impl ( uint8_t strength ) {
    if(!shaderLoaded){
        return nullptr;
    }

    if(darkSteps[strength] == nullptr){
        initDarkenTexture(strength);
    }
    
    return darkSteps[strength];
}

void libtrainsim::Video::imguiHandler::initDarkenTexture ( uint8_t strength ) {
    if(darkSteps[strength] != nullptr){
        return;
    }
    
    std::stringstream ss;
    ss << "darken-texture-default-" << strength;
    
    auto newTex = std::make_shared<libtrainsim::Video::texture>(ss.str());
    newTex->createFramebuffer({1,1});
    drawColor_impl(newTex,{0,0,0,static_cast<float>(strength)/255.0});

    
    darkSteps[strength] = newTex;
}


