#include "genericBackend.hpp"

using namespace libtrainsim::Video;

videoGeneric::~videoGeneric(){
    
}

libtrainsim::Video::videoGeneric::videoGeneric() {
}


videoGeneric::videoGeneric(std::shared_ptr<libtrainsim::Video::genericWindowManager> _window, std::shared_ptr<libtrainsim::Video::genericRenderer> _renderer):renderer{_renderer},window{_window}{};

std::shared_ptr<libtrainsim::Video::genericRenderer> videoGeneric::getRenderer(){
    return renderer;
}

std::shared_ptr<libtrainsim::Video::genericWindowManager> videoGeneric::getWindowManager(){
    return window;
}

bool videoGeneric::load(const std::filesystem::path& uri){
    return renderer->load(uri);
}

void videoGeneric::createWindow(const std::string& windowName){
    window->createWindow(windowName);
}

void videoGeneric::refreshWindow(){
    window->refreshWindow();
}

uint64_t videoGeneric::getFrameCount(){
    return renderer->getFrameCount();
}

void videoGeneric::gotoFrame(uint64_t frameNum){
    return window->gotoFrame(frameNum);
}

double videoGeneric::getHight(){
    return renderer->getHight();
}

double videoGeneric::getWidth(){
    return renderer->getWidth();
}

const std::filesystem::path& videoGeneric::getLoadedFile() const{
    return renderer->getLoadedFile();
}
