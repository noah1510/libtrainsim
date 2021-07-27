#include "backends/SDL2WindowManager.hpp"

#if defined(HAS_SDL_SUPPORT)

using namespace libtrainsim;
using namespace libtrainsim::Video;

libtrainsim::Video::SDL2WindowManager::SDL2WindowManager ( libtrainsim::Video::genericRenderer& _renderer ) : genericWindowManager{_renderer} {
    pict = std::make_shared<libtrainsim::Frame>();
};

SDL2WindowManager::~SDL2WindowManager(){
    SDL_DestroyRenderer(sdl_renderer);
    SDL_Quit();
}

void SDL2WindowManager::createWindow(const std::string& windowName){
    if(currentWindowName != "" || windowName == "" || renderer.reachedEndOfFile() || windowFullyCreated){
        return;
    }

    currentWindowName = windowName;
    screen = SDL_CreateWindow(
        currentWindowName.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        renderer.getWidth()/2,
        renderer.getHight()/2,
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
    sdl_renderer = SDL_CreateRenderer(screen, -1, renderFlags);
    
    texture = SDL_CreateTexture(
        sdl_renderer,
        SDL_PIXELFORMAT_YV12,
        SDL_TEXTUREACCESS_STREAMING,
        renderer.getWidth(),
        renderer.getHight()
    );
    
    renderer.initFrame(pict);
    
    lastFrame.reset();
    lastFrame = renderer.getNextFrame();
    
    windowFullyCreated = true;
}

void SDL2WindowManager::refreshWindow(){
    if(!windowFullyCreated){
        return;
    }
    
    pict.reset();
    pict = renderer.scaleFrame(lastFrame);

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = renderer.getWidth();
    rect.h = renderer.getHight();
    
    SDL_UpdateYUVTexture(
        texture,
        &rect,
        pict->dataFF()->data[0],
        pict->dataFF()->linesize[0],
        pict->dataFF()->data[1],
        pict->dataFF()->linesize[1],
        pict->dataFF()->data[2],
        pict->dataFF()->linesize[2]
    );

    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer,texture,NULL,NULL);
    SDL_RenderPresent(sdl_renderer);
}

#endif
