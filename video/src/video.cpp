#include "video.hpp"
#include <iostream>
#include <memory>

libtrainsim::video::video(libtrainsim::Video::VideoBackendDefinition backend):currentBackend{backend}{}

#ifdef HAS_SDL_SUPPORT
void libtrainsim::video::initSDL2(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
}
#endif

#ifdef HAS_GLFW_SUPPORT
void libtrainsim::video::initGLFW3(){
    if (glfwInit() != GLFW_TRUE){
        std::cerr << "Error while starting glfw3!" << std::endl;
    }
}
#endif

std::string libtrainsim::video::hello_impl() const{
    return "Hello from the video singleton";
}

void libtrainsim::video::reset(){
    currentBackend_impl = nullptr;
    
    #if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_SDL_SUPPORT)
    if(currentBackend == libtrainsim::Video::VideoBackends::ffmpeg_SDL2){
        SDL_Quit();
    }
    #endif

    std::cout << "libtrainsim::video was reset" << std::endl;
}

bool libtrainsim::video::load_impl(const std::filesystem::path& uri){
    checkBackend_impl();
    return currentBackend_impl->load(uri);
}

const std::filesystem::path& libtrainsim::video::getFilePath_impl() const{
    checkBackend_impl();
    return currentBackend_impl->getLoadedFile();
}

  
