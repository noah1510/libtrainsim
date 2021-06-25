#include "backends/ffmpeg_sdl.hpp"
#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::Video;
using namespace libtrainsim::backend;

#if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_SDL_SUPPORT)

videoFF_SDL::videoFF_SDL() : window{rendererFF}{}

videoFF_SDL::~videoFF_SDL(){
}

bool videoFF_SDL::load(const std::filesystem::path& uri){
    return rendererFF.load(uri);; 
}

void videoFF_SDL::createWindow(const std::string& windowName){
    window.createWindow(windowName);
}

void videoFF_SDL::refreshWindow(){
    window.refreshWindow();
}

void videoFF_SDL::gotoFrame(uint64_t frameNum){
    window.gotoFrame(frameNum);
}

uint64_t videoFF_SDL::getFrameCount(){
    return rendererFF.getFrameCount();
}
double videoFF_SDL::getHight(){
    return rendererFF.getHight();
}

double videoFF_SDL::getWidth(){
    return rendererFF.getWidth();
}

#endif
