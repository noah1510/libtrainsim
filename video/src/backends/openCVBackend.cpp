#include "backends/opencv.hpp"
#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::Video;

#ifdef HAS_OPENCV_SUPPORT

videoOpenCV::videoOpenCV(): videoGeneric{windowCV,rendererCV}{};

videoOpenCV::~videoOpenCV(){}

double videoOpenCV::getVideoProperty(const cv::VideoCaptureProperties& prop)const{
    return rendererCV.getVideoProperty(prop);
}

bool videoOpenCV::setVideoProperty(const cv::VideoCaptureProperties& prop, double value){
     return rendererCV.setVideoProperty(prop, value);
}

cv::VideoCaptureAPIs videoOpenCV::getBackend(){
    return rendererCV.getBackend();
}

void videoOpenCV::setBackend(cv::VideoCaptureAPIs newBackend){
    return rendererCV.setBackend(newBackend);
}

#endif
