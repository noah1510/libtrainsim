#include "frame.hpp"

using namespace libtrainsim;

Frame::Frame(){};

VideoBackends Frame::getBackend() const{
    return currentBackend;
}

bool Frame::isEmpty() const {
    switch (currentBackend){
        #ifdef HAS_FFMPEG_SUPPORT
            case(ffmpeg):
            case(ffmpeg_sdl):
                return frameDataFF==nullptr;
        #endif
        #ifdef HAS_OPENCV_SUPPORT
            case(opencv):
                return frameDataCV.empty();
        #endif // HAS_OPENCV_SUPPORT

        case(none):
        default:
            return true;
    }
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
    //clear();
}

void Frame::clear(){
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
    #ifdef HAS_SDL_SUPPORT
    if(currentBackend == ffmpeg_sdl){
        clearFF();
        return;
    }
    #endif
    #endif
}
