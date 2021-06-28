#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::Video;

#ifdef HAS_FFMPEG_SUPPORT

Frame::Frame(AVFrame* dat){
    currentBackend = renderer_ffmpeg;
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
    currentBackend = renderer_ffmpeg;
    if (x == frameDataFF){return *this;};
    if(frameDataFF != nullptr){
        av_frame_free(&frameDataFF);
        av_free(frameDataFF);
    }
    frameDataFF = x;
    return *this;
}

#endif
