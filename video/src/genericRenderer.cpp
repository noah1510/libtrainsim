#include "genericRenderer.hpp"

using namespace libtrainsim::Video;

genericRenderer::~genericRenderer(){
    
}

bool genericRenderer::load(const std::filesystem::path& uri){
    loadedFile = uri;

    return true;
}

std::shared_ptr<libtrainsim::Frame> genericRenderer::getNextFrame(){    
    return std::make_shared<libtrainsim::Frame>();
}

uint64_t genericRenderer::getFrameCount(){
    return 0;
}

std::shared_ptr<libtrainsim::Frame> genericRenderer::gotoFrame(uint64_t frameNum){
    return std::make_shared<libtrainsim::Frame>();
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

std::shared_ptr<libtrainsim::Frame> libtrainsim::Video::genericRenderer::scaleFrame(std::shared_ptr<libtrainsim::Frame> frame){
    return frame;
}

void libtrainsim::Video::genericRenderer::initFrame(std::shared_ptr<libtrainsim::Frame> frame){
    return;
}

