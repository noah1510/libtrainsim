#include "backends/generic.hpp"

using namespace libtrainsim::backend;

videoGeneric::~videoGeneric(){
    
}

bool videoGeneric::load(const std::filesystem::path& uri){
    loadedFile = uri;

    return true;
}

const libtrainsim::Frame videoGeneric::getNextFrame(){    
    return Frame();
}


void videoGeneric::createWindow(const std::string& windowName){
    if(currentWindowName != "" || windowName == ""){
        return;
    }

    currentWindowName = windowName;
}

void videoGeneric::refreshWindow(){
    if(currentWindowName != ""){std::cout << "refreshing the window" << std::endl;};
}

void videoGeneric::displayFrame(const Frame& newFrame){
    if (newFrame.getBackend() != libtrainsim::opencv || newFrame.dataCV().empty()){
        return;
    } 

    lastFrame = newFrame;
    refreshWindow();
}

void videoGeneric::updateWindow(){
    displayFrame(getNextFrame());
}

uint64_t videoGeneric::getFrameCount(){
    return 0;
}

void videoGeneric::gotoFrame(uint64_t frameNum){
    return;
}

double videoGeneric::getHight(){
    return 0.0;
}

double videoGeneric::getWidth(){
    return 0.0;
}

const std::filesystem::path& videoGeneric::getLoadedFile() const{
    return loadedFile;
}
