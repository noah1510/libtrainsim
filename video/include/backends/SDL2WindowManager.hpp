#pragma once

#include "genericWindowManager.hpp"

namespace libtrainsim {
    namespace Video{
        #if defined(HAS_SDL_SUPPORT)
            class SDL2WindowManager : public genericWindowManager{
            private:
                SDL_Window* screen = nullptr;
                SDL_Renderer* sdl_renderer = nullptr;
                SDL_Texture* texture = nullptr;
                
                Frame pict;
            public:
                SDL2WindowManager(genericRenderer& _renderer);
                ~SDL2WindowManager();
                void createWindow(const std::string& windowName);
                void refreshWindow();
            };
        #endif
    }
}
