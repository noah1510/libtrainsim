#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::Video;

#ifdef HAS_OPENCV_SUPPORT

Frame::Frame(const cv::UMat& frameDat){
    currentBackend = renderer_opencv;
    frameDataCV = frameDat;
}

cv::UMat Frame::dataCV() const{
    if(currentBackend != renderer_opencv){
        return cv::UMat();
    }

    return frameDataCV.clone();
}

void Frame::clearCV(){
    frameDataCV.release();
}

Frame::operator cv::UMat(){
    return frameDataCV;
}

#endif
