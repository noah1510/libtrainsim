#include "genericRenderer.hpp"

using namespace libtrainsim::Video;

genericRenderer::~genericRenderer(){
    
}

bool genericRenderer::load(const std::filesystem::path& uri){
    loadedFile = uri;

    return true;
}

const libtrainsim::Frame genericRenderer::getNextFrame(){    
    return Frame();
}

uint64_t genericRenderer::getFrameCount(){
    return 0;
}

const libtrainsim::Frame genericRenderer::gotoFrame(uint64_t frameNum){
    return Frame();
}

double genericRenderer::getHight(){
    return 0.0;
}

double genericRenderer::getWidth(){
    return 0.0;
}

const std::filesystem::path& genericRenderer::getLoadedFile() const{
    return loadedFile;
}

bool libtrainsim::Video::genericRenderer::reachedEndOfFile() {
    return endOfFile;
}

libtrainsim::Frame libtrainsim::Video::genericRenderer::scaleFrame(const libtrainsim::Frame& frame){
    return frame;
}

void libtrainsim::Video::genericRenderer::initFrame(libtrainsim::Frame& frame){
    return;
}

