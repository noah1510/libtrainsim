#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::Video;

Frame::Frame(){};

RendererBackends Frame::getBackend() const{
    return currentBackend;
}

bool Frame::isEmpty() const {
    switch (currentBackend){
        #ifdef HAS_FFMPEG_SUPPORT
            case(renderer_ffmpeg):
                return frameDataFF==nullptr;
        #endif
        #ifdef HAS_OPENCV_SUPPORT
            case(renderer_opencv):
                return frameDataCV.empty();
        #endif // HAS_OPENCV_SUPPORT

        case(renderer_none):
        default:
            return true;
    }
}

void Frame::setBackend ( RendererBackends newBackend ) {
    if(currentBackend == renderer_none){
        #ifdef HAS_FFMPEG_SUPPORT
        if (newBackend == renderer_ffmpeg){
            createEmptyFF();
        }
        #endif
        currentBackend = newBackend;
    }
}

Frame::~Frame(){
    clear();
}

void Frame::clear(){
    if(currentBackend == renderer_none){return;};
    
    #ifdef HAS_OPENCV_SUPPORT
    if(currentBackend == renderer_opencv){
        clearCV();
        return;
    }
    #endif
    
    #ifdef HAS_FFMPEG_SUPPORT
    if(currentBackend == renderer_ffmpeg){
        clearFF();
        return;
    }
    #endif
}
