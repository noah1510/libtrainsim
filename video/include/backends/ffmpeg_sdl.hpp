#pragma once

#include "genericBackend.hpp"
#include "ffmpegRenderer.hpp"
#include "SDL2WindowManager.hpp"

#include <filesystem>
#include <string>

namespace libtrainsim{
    namespace Video{
        #if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_SDL_SUPPORT)
        
        class videoFF_SDL : public videoGeneric{
            private:

            public:

                videoFF_SDL();
                
                /**
                * @brief Destroy the video FF_SDL object
                * 
                */
                ~videoFF_SDL();
        };

        #endif

    }
}
