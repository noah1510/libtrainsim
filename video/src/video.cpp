#include "video.hpp"
#include <iostream>
#include <memory>

libtrainsim::Video::videoManager::videoManager(){
    try{
        initSDL2();
        currentBackend_impl = std::make_unique<libtrainsim::Video::videoFF_SDL>();
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not create instance"));
    }
}

libtrainsim::Video::videoManager::~videoManager(){
    reset();
}

void libtrainsim::Video::videoManager::initSDL2(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
}

void libtrainsim::Video::videoManager::reset(){
    currentBackend_impl = nullptr;
    
    SDL_Quit();

    std::cout << "libtrainsim::video was reset" << std::endl;
}

bool libtrainsim::Video::videoManager::load(const std::filesystem::path& uri){
    return currentBackend_impl->load(uri);
}

const std::filesystem::path& libtrainsim::Video::videoManager::getFilePath() const{
    return currentBackend_impl->getLoadedFile();
}

void libtrainsim::Video::videoManager::createWindow ( const std::string& windowName ) {
    currentBackend_impl->createWindow(windowName);
}

double libtrainsim::Video::videoManager::getHeight() {
    return currentBackend_impl->getHeight();
}

double libtrainsim::Video::videoManager::getWidth() {
    return currentBackend_impl->getWidth();
}

void libtrainsim::Video::videoManager::gotoFrame ( double frame_num ) {
    currentBackend_impl->gotoFrame(frame_num);
}

bool libtrainsim::Video::videoManager::reachedEndOfFile() {
    return currentBackend_impl->getRenderer()->reachedEndOfFile();
}

void libtrainsim::Video::videoManager::refreshWindow() {
    currentBackend_impl->refreshWindow();
}






  
