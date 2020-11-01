#include "frame.hpp"

using namespace libtrainsim;

#ifdef HAS_OPENCV_SUPPORT

Frame::Frame(const cv::UMat& frameDat){
    currentBackend = opencv;
    frameDataCV = frameDat;
}

cv::UMat Frame::dataCV() const{
    if(currentBackend != opencv){
        return cv::UMat();
    }

    return frameDataCV.clone();
}

#endif