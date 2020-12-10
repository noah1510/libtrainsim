#include "video.hpp"
#include <iostream>
#include <memory>

libtrainsim::video::video(VideoBackends backend){
    currentBackend = backend;
    #ifdef HAS_SDL_SUPPORT
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    #endif
}

std::string libtrainsim::video::hello_impl() const{
    return "Hello from the video singleton";
}

void libtrainsim::video::reset(){
    currentBackend_impl = nullptr;
    
    #ifdef HAS_OPENCV_SUPPORT
    if (currentBackend == opencv){
        cv::destroyAllWindows();
    }
    #endif
    
    #ifdef HAS_FFMPEG_SUPPORT
    if(currentBackend == ffmpeg_sdl){
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

  
