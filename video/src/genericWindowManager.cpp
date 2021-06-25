#include "genericWindowManager.hpp"


using namespace libtrainsim::Video;

genericWindowManager::genericWindowManager(genericRenderer& _renderer): renderer{_renderer}{};

genericWindowManager::~genericWindowManager(){
    
}

void genericWindowManager::createWindow(const std::string& windowName){
    if(currentWindowName != "" || windowName == ""){
        return;
    }

    currentWindowName = windowName;
}

void genericWindowManager::refreshWindow(){
    if(currentWindowName != ""){std::cout << "refreshing the window" << std::endl;};
}

void genericWindowManager::displayFrame(const Frame& newFrame){
    if (newFrame.isEmpty()){
        return;
    }

    lastFrame = newFrame;
    refreshWindow();
}


void genericWindowManager::gotoFrame(uint64_t frameNum){
    return displayFrame(renderer.gotoFrame(frameNum));
}

