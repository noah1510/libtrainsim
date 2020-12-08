#include "frame.hpp"

using namespace libtrainsim;

Frame::Frame(){};

VideoBackends Frame::getBackend() const{
    return currentBackend;
}

void Frame::setBackend ( VideoBackends newBackend ) {
    if(currentBackend == none){
        #ifdef HAS_FFMPEG_SUPPORT
        if (newBackend == ffmpeg){
            createEmptyFF();
        }else{
            #ifdef HAS_SDL_SUPPORT
            if(newBackend == ffmpeg_sdl){createEmptyFF();};
            #endif
        }
        #endif
        currentBackend = newBackend;
    }
}

Frame::~Frame(){
    if(currentBackend == none){return;};
    
    #ifdef HAS_OPENCV_SUPPORT
    if(currentBackend == opencv){
        clearCV();
        return;
    }
    #endif
    
    #ifdef HAS_FFMPEG_SUPPORT
    if(currentBackend == ffmpeg){
        clearFF();
        return;
    }
    #endif
}
