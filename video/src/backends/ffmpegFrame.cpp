#include "backends/ffmpegFrame.hpp"

#ifdef HAS_FFMPEG_SUPPORT

libtrainsim::ffmpegFrame::ffmpegFrame(AVFrame* dat){
    frameDataFF = dat;
}

AVFrame* libtrainsim::ffmpegFrame::dataFF() const{
    return frameDataFF;
}

libtrainsim::ffmpegFrame::ffmpegFrame() {
    frameDataFF = av_frame_alloc();
}

libtrainsim::ffmpegFrame::operator AVFrame*(){
    return frameDataFF;
}

libtrainsim::ffmpegFrame& libtrainsim::ffmpegFrame::operator=(AVFrame* x){
    
    if (x == frameDataFF){return *this;};
    if(frameDataFF != nullptr){
        av_frame_free(&frameDataFF);
        av_free(frameDataFF);
    }
    
    frameDataFF = x;
    return *this;
}

bool libtrainsim::ffmpegFrame::isEmpty() const {
    return frameDataFF==nullptr;
}

void libtrainsim::ffmpegFrame::clear(){
    av_frame_free(&frameDataFF);
    av_free(frameDataFF);
    frameDataFF = av_frame_alloc();
}


#endif
