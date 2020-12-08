#include "frame.hpp"

using namespace libtrainsim;

#ifdef HAS_FFMPEG_SUPPORT

Frame::Frame(AVFrame* dat){
    currentBackend = ffmpeg;
    frameDataFF = dat;
}

AVFrame* Frame::dataFF(){
    if(currentBackend < 2 || currentBackend > 3){return nullptr;};

    return frameDataFF;
}

void Frame::clearFF(){
    if(currentBackend < 2 || currentBackend > 3){return;};
    av_frame_free(&frameDataFF);
    av_free(frameDataFF);
}

void Frame::createEmptyFF(){
    frameDataFF = av_frame_alloc();
}

#endif
