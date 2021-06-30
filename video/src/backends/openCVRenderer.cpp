#include "backends/openCVRenderer.hpp"

using namespace libtrainsim;
using namespace libtrainsim::Video;

#if defined(HAS_OPENCV_SUPPORT)

openCVRenderer::~openCVRenderer(){

}

bool openCVRenderer::load(const std::filesystem::path& uri){
    loadedFile = uri;

    videoCap = std::make_unique<cv::VideoCapture>(loadedFile.string(),backend);
    if(!videoCap->isOpened()){
        return false;
    }
    
    endOfFile = false;

    return true;
}

const libtrainsim::Frame openCVRenderer::getNextFrame(){
    if(endOfFile){return Frame();};
    
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


const libtrainsim::Frame openCVRenderer::gotoFrame(uint64_t frameNum){
    if(endOfFile){return Frame();};
    setVideoProperty(cv::CAP_PROP_POS_FRAMES, frameNum);
    
    return getNextFrame();
}

uint64_t openCVRenderer::getFrameCount(){
    if(endOfFile){return 0;};
    return getVideoProperty(cv::CAP_PROP_FRAME_COUNT);
}
double openCVRenderer::getHight(){
    if(endOfFile){return 0.0;};
    return getVideoProperty(cv::CAP_PROP_FRAME_HEIGHT);
}

double openCVRenderer::getWidth(){
    if(endOfFile){return 0.0;};
    return getVideoProperty(cv::CAP_PROP_FRAME_WIDTH);
}

double openCVRenderer::getVideoProperty(const cv::VideoCaptureProperties& prop)const{
    if(videoCap->isOpened()){
        return videoCap->get(prop);
    }

    return 0.0f;
}

bool openCVRenderer::setVideoProperty(const cv::VideoCaptureProperties& prop, double value){
     if(videoCap->isOpened()){
        return videoCap->set(prop,value);
    }

    return false;
}

cv::VideoCaptureAPIs openCVRenderer::getBackend(){
    return backend;
}

void openCVRenderer::setBackend(cv::VideoCaptureAPIs newBackend){
    backend = newBackend;
}

#endif


