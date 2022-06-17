#include "video.hpp"
#include <iostream>
#include <memory>

libtrainsim::video::video(){}

void libtrainsim::video::initSDL2(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
}

std::string libtrainsim::video::hello_impl() const{
    return "Hello from the video singleton";
}

void libtrainsim::video::reset(){
    currentBackend_impl = nullptr;
    
    SDL_Quit();

    std::cout << "libtrainsim::video was reset" << std::endl;
}

bool libtrainsim::video::load_impl(const std::filesystem::path& uri){
    try{checkBackend_impl();} catch(...){std::throw_with_nested(std::runtime_error("error loading backend implementation"));};
    return currentBackend_impl->load(uri);
}

const std::filesystem::path& libtrainsim::video::getFilePath_impl() const{
    try{checkBackend_impl();} catch(...){std::throw_with_nested(std::runtime_error("error loading backend implementation"));};
    return currentBackend_impl->getLoadedFile();
}

  
