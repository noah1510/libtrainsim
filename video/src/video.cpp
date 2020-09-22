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

    videoCap = std::make_unique<cv::VideoCapture>(static_cast<std::string>(loadedFile));
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


double libtrainsim::video::getFPS_impl(){
    if(videoCap->isOpened()){
        return videoCap->get(cv::CAP_PROP_FPS);
    }

    return 0.0f;
}

double libtrainsim::video::getPosMsec_impl(){
    if(videoCap->isOpened()){
        return videoCap->get(cv::CAP_PROP_POS_MSEC);
    }

    return 0.0f;
}

double libtrainsim::video::getPosFrames_impl(){
    if(videoCap->isOpened()){
        return videoCap->get(cv::CAP_PROP_POS_FRAMES);
    }

    return 0.0f;
}

double libtrainsim::video::getWidth_impl(){
    if(videoCap->isOpened()){
        return videoCap->get(cv::CAP_PROP_FRAME_WIDTH);
    }

    return 0.0f;
}

double libtrainsim::video::getHight_impl(){
    if(videoCap->isOpened()){
        return videoCap->get(cv::CAP_PROP_FRAME_HEIGHT);
    }

    return 0.0f;
 }

 double libtrainsim::video::getFrameCount_impl(){
    if(videoCap->isOpened()){
        return videoCap->get(cv::CAP_PROP_FRAME_COUNT);
    }

    return 0.0f;
 }
  