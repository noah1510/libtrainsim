#pragma once

#include "generic.hpp"
#include "frame.hpp"
#include "ffmpegRenderer.hpp"
#include "SDL2WindowManager.hpp"

#include <filesystem>
#include <string>

namespace libtrainsim{
    namespace backend{
        #if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_SDL_SUPPORT)
        
        class videoFF_SDL : public videoGeneric{
            private:
                libtrainsim::Video::ffmpegRenderer rendererFF{};
                
                libtrainsim::Video::SDL2WindowManager windowSDL{rendererFF};

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
