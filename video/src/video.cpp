#include "video.hpp"
#include <iostream>
#include <memory>

libtrainsim::video::video(void){

}

void libtrainsim::video::hello_impl() const{
    std::cout << "Hello from the video singleton" << std::endl;
}

void libtrainsim::video::reset(){
    std::cout << "libtrainsim::video was reset" << std::endl;
}

bool libtrainsim::video::load_impl(const std::filesystem::path& uri){
    loadedFile = uri;

    videoCap = std::make_unique<cv::VideoCapture>(static_cast<std::string>(loadedFile));
    if(!videoCap->isOpened()){
        return false;
    }

    return true;
}

double libtrainsim::video::getFPS_impl(){
    if(videoCap->isOpened()){
        return videoCap->get(cv::CAP_PROP_FPS);
    }

    return 0.0f;
}

const cv::Mat libtrainsim::video::getNextFrame_impl(){
    cv::Mat frame;
    auto status = videoCap->read(frame);

    if (!status){
        return cv::Mat();
    }
    
    return frame.clone();
}

  