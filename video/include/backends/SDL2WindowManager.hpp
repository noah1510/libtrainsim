#pragma once

#include "genericWindowManager.hpp"

#ifdef HAS_SDL_SUPPORT
    #if  __has_include("SDL2/SDL.h") && __has_include("SDL2/SDL_thread.h")
        #include <SDL2/SDL.h>
        #include <SDL2/SDL_thread.h>
    #else
        #undef HAS_SDL_SUPPORT
    #endif
#endif


namespace libtrainsim {
    namespace Video{
        #if defined(HAS_SDL_SUPPORT)
            class SDL2WindowManager : public genericWindowManager{
            private:
                SDL_Window* screen = nullptr;
                SDL_Renderer* sdl_renderer = nullptr;
                SDL_Texture* texture = nullptr;
                
                std::shared_ptr<libtrainsim::Frame> pict;
            public:
                SDL2WindowManager(genericRenderer& _renderer);
                ~SDL2WindowManager();
                void createWindow(const std::string& windowName);
                void refreshWindow();
            };
        #endif
    }
}
