#include "backends/openCVWindowManager.hpp"

#if defined(HAS_OPENCV_SUPPORT)

using namespace libtrainsim;
using namespace libtrainsim::Video;

libtrainsim::Video::openCVWindowManager::openCVWindowManager ( libtrainsim::Video::openCVRenderer& _renderer ) : genericWindowManager{_renderer} {};

openCVWindowManager::~openCVWindowManager(){
    if(currentWindowName != ""){
        cv::destroyWindow(currentWindowName);
    }
}

void openCVWindowManager::createWindow(const std::string& windowName){
    if(currentWindowName != "" || windowName == "" || renderer.reachedEndOfFile() || windowFullyCreated){
        return;
    }

    currentWindowName = windowName;
    cv::namedWindow(currentWindowName, cv::WINDOW_AUTOSIZE);
    
    lastFrame->clear();
    lastFrame = renderer.getNextFrame();
    
    windowFullyCreated = true;
}

void openCVWindowManager::refreshWindow(){
    if(!windowFullyCreated){
        return;
    }
    
    if(currentWindowName != ""){
        cv::imshow(currentWindowName, lastFrame->dataCV());
    }
}

#endif

