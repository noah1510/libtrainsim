#include "video.hpp"
#include <iostream>

libtrainsim::video::video(void){

}

void libtrainsim::video::hello_impl() const{
    std::cout << "Hello from the video singleton" << std::endl;
}

void libtrainsim::video::reset(){
    std::cout << "libtrainsim::video was reset" << std::endl;
}



bool libtrainsim::video::load_impl(const std::filesystem::path& uri){
    loadedFile = uri;

    return true;
}

  