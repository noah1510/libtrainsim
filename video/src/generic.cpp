#include "generic.hpp"

using namespace libtrainsim::backend;

videoGeneric::~videoGeneric(){
    
}

videoGeneric::videoGeneric(libtrainsim::Video::genericWindowManager& _window, libtrainsim::Video::genericRenderer& _renderer):renderer{_renderer},window{_window}{};

bool videoGeneric::load(const std::filesystem::path& uri){
    return renderer.load(uri);
}

void videoGeneric::createWindow(const std::string& windowName){
    window.createWindow(windowName);
}

void videoGeneric::refreshWindow(){
    window.refreshWindow();
}

uint64_t videoGeneric::getFrameCount(){
    return renderer.getFrameCount();
}

void videoGeneric::gotoFrame(uint64_t frameNum){
    return window.gotoFrame(frameNum);
}

double videoGeneric::getHight(){
    return renderer.getHight();
}

double videoGeneric::getWidth(){
    return renderer.getWidth();
}

const std::filesystem::path& videoGeneric::getLoadedFile() const{
    return renderer.getLoadedFile();
}
