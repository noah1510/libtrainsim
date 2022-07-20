#include "frame.hpp"

libtrainsim::Video::Frame::~Frame() {
    av_frame_free(&frameData);
    av_free(frameData);
}

libtrainsim::Video::Frame::Frame(AVFrame* dat){
    frameData = dat;
}

AVFrame* libtrainsim::Video::Frame::data() const{
    return frameData;
}

libtrainsim::Video::Frame::Frame() {
    frameData = av_frame_alloc();
}

libtrainsim::Video::Frame::operator AVFrame*(){
    return frameData;
}

libtrainsim::Video::Frame& libtrainsim::Video::Frame::operator=(AVFrame* x){
    
    if (x == frameData){return *this;};
    if(frameData != nullptr){
        av_frame_free(&frameData);
        av_free(frameData);
    }
    
    frameData = x;
    return *this;
}

bool libtrainsim::Video::Frame::isEmpty() const {
    return frameData==nullptr;
}

void libtrainsim::Video::Frame::clear(){
    av_frame_free(&frameData);
    av_free(frameData);
    frameData = av_frame_alloc();
}
