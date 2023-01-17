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

void libtrainsim::Video::videoManager::createWindow ( const std::string& windowName, const std::filesystem::path& shaderLocation, const std::filesystem::path& textureLocation ) {
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
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create the output framebuffer"));
    }
    
    try{
        displayShader = std::make_shared<libtrainsim::Video::Shader>(shaderLocation/"display.vert", shaderLocation/"display.frag");
        
        displayShader->use();
        std::vector<int> units {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
        displayShader->setUniform("tex", units);
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not create output shader"));
    }
    
    try{
        auto bg_tex = std::make_shared<texture>("background"s);
        addTexture(bg_tex);
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not init video image"));
    }

    currentWindowName = windowName;
    
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
    decode->requestFrame(frame_num);
}

bool libtrainsim::Video::videoManager::reachedEndOfFile() {
    return decode->reachedEndOfFile();
    //return decode.reachedEndOfFile();
}

void libtrainsim::Video::videoManager::updateOutput() {
    
    outputBuffer->loadFramebuffer();
    
    displayShader->use();
    displayShader->setUniform("transform", outputBuffer->getProjection());
    displayShader->setUniform("enabledUnits", displayTextures.size());
    
    for(unsigned int i = 0; i < displayTextures.size(); i++){
        displayTextures[i]->bind(i);
    }
    
    imguiHandler::drawRect();
}


void libtrainsim::Video::videoManager::refreshWindow() {
    if(!windowFullyCreated){
        return;
    }
    
    displayTextures[0]->updateImage(decode->getUsableFramebufferBuffer(), decode->getDimensions());

    //draw the render window
    
    //set size and pos on program start to initial values
    static bool firstStart = true;
    if(firstStart){
        ImGui::SetNextWindowContentSize( decode->getDimensions() );
        
        ImVec2 initialPos {0, ImGui::GetStyle().DisplayWindowPadding.y};
        ImGui::SetNextWindowPos(initialPos);
        
        firstStart = false;
    }
    
    //actually start drawing the window
    ImGui::Begin(currentWindowName.c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav);
        
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
}

void libtrainsim::Video::videoManager::removeTexture ( const std::string& textureName ) {
    if(textureName == "background"){
        throw std::invalid_argument("the background texture cannot be removed");
    }
    
    for(auto i = displayTextures.begin(); i < displayTextures.end(); i++){
        if((*i)->getName() == textureName){
            displayTextures.erase(i);
            return;
        }
    }
    
    throw std::invalid_argument("a texture with the name '" + textureName + "' does not exist");
}

std::optional<std::vector<sakurajin::unit_system::time_si>> libtrainsim::Video::videoManager::getNewRendertimes() {
    return decode->getNewRendertimes();
}


