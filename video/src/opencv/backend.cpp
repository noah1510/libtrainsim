#include "backends/opencv.hpp"
#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::backend;

#ifdef HAS_OPENCV_SUPPORT

videoOpenCV::~videoOpenCV(){
    if(currentWindowName != ""){
        cv::destroyWindow(currentWindowName);
    }
}

bool videoOpenCV::load(const std::filesystem::path& uri){
    return renderer.load(uri);
}

const libtrainsim::Frame videoOpenCV::getNextFrame(){
    return renderer.getNextFrame();
}

double videoOpenCV::getVideoProperty(const cv::VideoCaptureProperties& prop)const{
    return renderer.getVideoProperty(prop);
}

bool videoOpenCV::setVideoProperty(const cv::VideoCaptureProperties& prop, double value){
     return renderer.setVideoProperty(prop, value);
}

cv::VideoCaptureAPIs videoOpenCV::getBackend(){
    return renderer.getBackend();
}

void videoOpenCV::setBackend(cv::VideoCaptureAPIs newBackend){
    return renderer.setBackend(newBackend);
}

void videoOpenCV::createWindow(const std::string& windowName){
    if(currentWindowName != "" || windowName == ""){
        return;
    }

    currentWindowName = windowName;
    cv::namedWindow(currentWindowName, cv::WINDOW_AUTOSIZE);
}

void videoOpenCV::refreshWindow(){
    if(currentWindowName != ""){
        cv::imshow(currentWindowName, lastFrame.dataCV());
    }
}

void videoOpenCV::displayFrame(const Frame& newFrame){
    if (newFrame.getBackend() != libtrainsim::Video::renderer_opencv || newFrame.dataCV().empty()){
        return;
    } 

    lastFrame = newFrame;
    refreshWindow();
}

void videoOpenCV::gotoFrame(uint64_t frameNum){
    displayFrame(renderer.gotoFrame(frameNum));
}

uint64_t videoOpenCV::getFrameCount(){
    return renderer.getFrameCount();
}

double videoOpenCV::getHight(){
    return renderer.getHight();
}

double videoOpenCV::getWidth(){
    return renderer.getWidth();
}

#endif
