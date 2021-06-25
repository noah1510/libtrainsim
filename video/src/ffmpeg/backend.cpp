#include "backends/ffmpeg_sdl.hpp"
#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::backend;

#if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_SDL_SUPPORT)

videoFF_SDL::~videoFF_SDL(){
    lastFrame.clear();
    pict.clear();

    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

bool videoFF_SDL::load(const std::filesystem::path& uri){
    rendererFF.load(uri);
    
    lastFrame = Frame();
    lastFrame.setBackend(ffmpeg);

    return true; 
}

const libtrainsim::Frame videoFF_SDL::getNextFrame(){
    return rendererFF.getNextFrame();
}


void videoFF_SDL::createWindow(const std::string& windowName){
    if(currentWindowName != "" || windowName == "" || rendererFF.reachedEndOfFile() || windowFullyCreated){
        return;
    }

    currentWindowName = windowName;
    screen = SDL_CreateWindow(
        currentWindowName.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        rendererFF.getWidth()/2,
        rendererFF.getHight()/2,
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE
    );

    if (!screen){
        std::cerr << "SDL: could not set video mode - exiting." << std::endl;
        return;
    }
    
    SDL_GL_SetSwapInterval(1);
    auto renderFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
    #ifdef ENDABLE_VSYNC
    if(true){renderFlags |= SDL_RENDERER_PRESENTVSYNC;};
    #endif
    renderer = SDL_CreateRenderer(screen, -1, renderFlags);
    
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_YV12,
        SDL_TEXTUREACCESS_STREAMING,
        rendererFF.getWidth(),
        rendererFF.getHight()
    );
    
    rendererFF.initFrame(pict);
    
    lastFrame.clear();
    lastFrame = getNextFrame();
    
    windowFullyCreated = true;
}

void videoFF_SDL::refreshWindow(){
    if(!windowFullyCreated){
        return;
    }
    
    pict = rendererFF.scaleFrame(lastFrame);

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = rendererFF.getWidth();
    rect.h = rendererFF.getHight();
    
    SDL_UpdateYUVTexture(
        texture,
        &rect,
        pict.dataFF()->data[0],
        pict.dataFF()->linesize[0],
        pict.dataFF()->data[1],
        pict.dataFF()->linesize[1],
        pict.dataFF()->data[2],
        pict.dataFF()->linesize[2]
    );

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
}

void videoFF_SDL::displayFrame(const Frame& newFrame){
    if (!(newFrame.getBackend() == libtrainsim::ffmpeg_sdl || newFrame.getBackend() == libtrainsim::ffmpeg) ||
        newFrame.dataFF() == nullptr
    ){
        return;
    }

    lastFrame.clear();
    lastFrame = newFrame;
    refreshWindow();
}

void videoFF_SDL::gotoFrame(uint64_t frameNum){
    displayFrame(rendererFF.gotoFrame(frameNum));
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
