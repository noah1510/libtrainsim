#include "texture.hpp"

using namespace std::literals;

libtrainsim::Video::texture::texture() {
    std::scoped_lock lock{acessMutex};
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    #ifdef LIBTRAINSIM_PREFORMANCE_VIDEO
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    #else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    #endif
    
    glBindTexture(GL_TEXTURE_2D, 0);

    name = textureID;

}

libtrainsim::Video::texture::texture ( const std::string& _name ) : texture{} {
    std::scoped_lock lock{acessMutex};
    
    if(_name.empty() || _name.size() <= 1){
        throw std::invalid_argument("not a valid name for a texture:" + _name);
    }
    
    name = _name;
}

libtrainsim::Video::texture::texture ( const std::filesystem::path& URI ) :texture{} {
   
    if( !std::filesystem::exists( URI) ){
        throw std::invalid_argument("image file does not exist! " + URI.string());
    }
    
    std::scoped_lock<std::shared_mutex>{acessMutex};
    auto tmp_surface = IMG_Load(URI.string().c_str());
    if(!tmp_surface) {
        throw std::runtime_error(std::string{"Could not read image: "} + IMG_GetError());
    }
    
    auto* surface = SDL_ConvertSurfaceFormat(tmp_surface, SDL_PIXELFORMAT_RGBA32, 0);
    if(surface == nullptr){
        SDL_FreeSurface(tmp_surface);
        throw std::runtime_error(std::string{"Could not convert to rgba32: "} + SDL_GetError());
    }
    
    SDL_FreeSurface(tmp_surface);
    
    SDL_SetSurfaceRLE(surface, true);
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
    
    // set texture filtering parameters
    glBindTexture(GL_TEXTURE_2D ,textureID);
    #ifdef LIBTRAINSIM_PREFORMANCE_VIDEO
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    #else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    #endif
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    
    auto w = surface->w;
    auto h = surface->h;
    
    //upload the texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        w,
        h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        surface->pixels
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    SDL_FreeSurface(surface);
    
    imageSize.x() = w;
    imageSize.y() = h;
    
    
    name = URI.string();
}



libtrainsim::Video::texture::~texture(){
    std::scoped_lock lock{acessMutex};
    
    if(framebufferMode){
        glDeleteFramebuffers(1, &FBO);
    }
    glDeleteTextures(1, &textureID);
}

const libtrainsim::Video::dimensions & libtrainsim::Video::texture::getSize() noexcept {
    std::shared_lock lock{acessMutex};
    return imageSize;
}

unsigned int libtrainsim::Video::texture::getTextureId() noexcept {
    std::shared_lock lock{acessMutex};
    return textureID;
}

std::shared_mutex & libtrainsim::Video::texture::getMutex() noexcept {
    return acessMutex;
}

const std::string & libtrainsim::Video::texture::getName() noexcept {
    std::shared_lock lock{acessMutex};
    return name;
}


void libtrainsim::Video::texture::updateImage (const std::vector<uint8_t>& data, const libtrainsim::Video::dimensions& newSize ) {
    if(data.size() < newSize.x()*newSize.y()*4){
        throw std::invalid_argument("data size too small");
    }
    updateImage(data.data(),newSize);
}

void libtrainsim::Video::texture::updateImage (const uint8_t* data, const libtrainsim::Video::dimensions& newSize ) {
    std::scoped_lock lock{acessMutex};
    if( imageSize.isRoughly(newSize)){
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        auto [w,h] = imageSize;
        glTexSubImage2D(
            GL_TEXTURE_2D, 
            0, 
            0,
            0,
            w, 
            h, 
            GL_RGBA, 
            GL_UNSIGNED_BYTE, 
            data
        );
    }else{

        imageSize = newSize;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        auto [w,h] = imageSize;
        glTexImage2D(
            GL_TEXTURE_2D, 
            0, 
            GL_RGBA, 
            w, 
            h, 
            0,
            GL_RGBA, 
            GL_UNSIGNED_BYTE, 
            data
        );
    }
}

void libtrainsim::Video::texture::resize ( const libtrainsim::Video::dimensions& newSize, bool resetTexture ) {
    acessMutex.lock();

    if(!framebufferMode){
        if(!resetTexture){
            acessMutex.unlock();
            throw std::invalid_argument("Cannot change the size outside of framebuffer mode");
        }

        acessMutex.unlock();
        updateImage(nullptr, newSize);
        return;
    }
    
    imageSize = newSize;
    acessMutex.unlock();
}


void libtrainsim::Video::texture::bind(unsigned int unit) {
    std::scoped_lock lock{acessMutex};
    
    if(unit >= libtrainsim::Video::imguiHandler::getMaxTextureUnits()){
        std::stringstream ss;
        ss << "texture unit value too high. Only " << libtrainsim::Video::imguiHandler::getMaxTextureUnits() << " are allowed";
        throw std::invalid_argument(ss.str());
    }
    
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void libtrainsim::Video::texture::createFramebuffer ( const libtrainsim::Video::dimensions& framebufferSize ) {
    imguiHandler::warnOffThread();
    
    if(framebufferSize != imageSize){
        try{
            resize(framebufferSize, true);
        }catch(...){
             std::throw_with_nested(std::invalid_argument("Could not update the framebuffer size"));
        }

    }

    if(framebufferMode){
        return;
    }
    
    try{
        acessMutex.lock();
        
        //create the framebuffer for the output image
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
        
        unsigned int DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers);
        
        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(status != GL_FRAMEBUFFER_COMPLETE){
            std::stringstream ss;
            ss << "Could not create output framebuffer. Error: " << imguiHandler::decodeGLFramebufferStatus(status);
            throw std::runtime_error(ss.str());
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }catch(...){
        std::throw_with_nested(std::runtime_error("cannot create framebuffer"));
    }
    
    framebufferMode = true;
    imageSize = framebufferSize;
    acessMutex.unlock();
}

void libtrainsim::Video::texture::createFramebuffer(){
    try{
        createFramebuffer(imguiHandler::getDefaultFBOSize());
    }catch(...){
        std::throw_with_nested(std::runtime_error("cannot create framebuffer with default size"));
    }
}

void libtrainsim::Video::texture::loadFramebuffer() {
    std::scoped_lock lock{acessMutex};
    if(!framebufferMode){
        throw std::invalid_argument("Cannot load texture as framebuffer if is is not initialized as framebuffer");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    imguiHandler::setViewport(imageSize);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    imguiHandler::loadPerformanceGLOptions();
        
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendEquation(GL_MAX);
    
    auto clearColor = imguiHandler::getClearColor();
    glClearColor(clearColor.x,clearColor.y,clearColor.z,clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

bool libtrainsim::Video::texture::hasFramebuffer() const noexcept {
    return framebufferMode;
}

unsigned int libtrainsim::Video::texture::getFBO() const noexcept {
    return FBO;
}


void libtrainsim::Video::texture::displayImGui(const libtrainsim::Video::dimensions& displaySize) {
    std::shared_lock lock{acessMutex};
    
    auto displayS = displaySize;
    if(displayS.isRoughly({0.0f,0.0f})){
        displayS = imageSize;
    }
    
    ImGui::Image((void*)(intptr_t)textureID, displayS);
}

glm::mat4 libtrainsim::Video::texture::getProjection() noexcept {
    auto orth = glm::ortho(
        -1.0f, 
        1.0f,
        -1.0f,
        1.0f,
        -10.0f,
        10.0f
    );
    return orth;
}

