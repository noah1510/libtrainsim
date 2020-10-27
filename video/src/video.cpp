#include "video.hpp"
#include <iostream>
#include <memory>

libtrainsim::video::video(void){

}

std::string libtrainsim::video::hello_impl() const{
    return "Hello from the video singleton";
}

void libtrainsim::video::reset(){
    std::cout << "libtrainsim::video was reset" << std::endl;
}

bool libtrainsim::video::load_impl(const std::filesystem::path& uri){
    loadedFile = uri;

    videoCap = std::make_unique<cv::VideoCapture>(loadedFile.string(),backend);
    if(!videoCap->isOpened()){
        return false;
    }

    return true;
}

const cv::Mat libtrainsim::video::getNextFrame_impl(){
    cv::Mat frame;
    auto status = videoCap->read(frame);

    if (!status){
        return cv::Mat();
    }
    
    return frame.clone();
}

std::filesystem::path libtrainsim::video::getFilePath_impl() const{
    return loadedFile;
}

 double libtrainsim::video::getVideoProperty_impl(const cv::VideoCaptureProperties& prop)const{
    if(videoCap->isOpened()){
        return videoCap->get(prop);
    }

    return 0.0f;
 }

 bool libtrainsim::video::setVideoProperty_impl(const cv::VideoCaptureProperties& prop, double value){
     if(videoCap->isOpened()){
        return videoCap->set(prop,value);
    }

    return false;
 }
  