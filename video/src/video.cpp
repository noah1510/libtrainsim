#include "video.hpp"
#include <iostream>

libtrainsim::video::video(void){

}

void libtrainsim::video::hello_impl(){
    std::cout<<"Hello from the video singleton"<<std::endl;
}