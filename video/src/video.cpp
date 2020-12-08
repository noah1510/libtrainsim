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
    #ifdef HAS_OPENCV_SUPPORT
    backendCV = nullptr;
    if (currentBackend == opencv){
        cv::destroyAllWindows();
    }
    #endif

    std::cout << "libtrainsim::video was reset" << std::endl;
}

bool libtrainsim::video::load_impl(const std::filesystem::path& uri){
    loadedFile = uri;

    #ifdef HAS_OPENCV_SUPPORT
    if (currentBackend == opencv){
        if(backendCV == nullptr){backendCV = std::make_unique<libtrainsim::backend::videoOpenCV>();};
        return backendCV->load(uri);
    };
    #endif

    return true;
}

std::filesystem::path libtrainsim::video::getFilePath_impl() const{
    return loadedFile;
}

  
