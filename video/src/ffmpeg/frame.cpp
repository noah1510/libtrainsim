#include "frame.hpp"

using namespace libtrainsim;

#ifdef HAS_FFMPEG_SUPPORT

Frame::Frame(AVFrame* dat){
    currentBackend = ffmpeg;
    frameDataFF = dat;
}

AVFrame* Frame::dataFF() const{
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

Frame::operator AVFrame*(){
    if(currentBackend < 2 || currentBackend > 3){return nullptr;};
    return frameDataFF;
}

Frame& Frame::operator=(AVFrame* x){
    currentBackend = ffmpeg;
    if(frameDataFF != nullptr){
        av_frame_free(&frameDataFF);
        av_free(frameDataFF);
    }
    frameDataFF = x;
    return *this;
}

#endif
