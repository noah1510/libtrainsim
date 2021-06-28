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
    loadedFile = uri;

    videoCap = std::make_unique<cv::VideoCapture>(loadedFile.string(),backend);
    if(!videoCap->isOpened()){
        return false;
    }

    return true;
}

const libtrainsim::Frame videoOpenCV::getNextFrame(){
    cv::UMat frame;
    auto status = videoCap->grab();
    if(!status){
        return cv::UMat();
    }

    status = videoCap->retrieve(frame);
    if (!status){
        return cv::UMat();
    }
    
    return frame.clone();
}

double videoOpenCV::getVideoProperty(const cv::VideoCaptureProperties& prop)const{
    if(videoCap->isOpened()){
        return videoCap->get(prop);
    }

    return 0.0f;
}

bool videoOpenCV::setVideoProperty(const cv::VideoCaptureProperties& prop, double value){
     if(videoCap->isOpened()){
        return videoCap->set(prop,value);
    }

    return false;
}

cv::VideoCaptureAPIs videoOpenCV::getBackend(){
    return backend;
}

void videoOpenCV::setBackend(cv::VideoCaptureAPIs newBackend){
    backend = newBackend;
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
    setVideoProperty(cv::CAP_PROP_POS_FRAMES, frameNum);
    displayFrame(getNextFrame());
}

uint64_t videoOpenCV::getFrameCount(){
    return getVideoProperty(cv::CAP_PROP_FRAME_COUNT);
}

double videoOpenCV::getHight(){
    return getVideoProperty(cv::CAP_PROP_FRAME_HEIGHT);
}

double videoOpenCV::getWidth(){
    return getVideoProperty(cv::CAP_PROP_FRAME_WIDTH);
}

#endif
