#pragma once

#include "genericBackend.hpp"
#include "backends/ffmpeg_sdl.hpp"

namespace libtrainsim {
    
    namespace Video{
        enum WindowingBackends{
            /// No selected backend
            window_none = 0,
            
            #ifdef HAS_SDL_SUPPORT
            ///use ffmpeg with any found windowing system
            window_sdl = 2,
            #endif
        };

        class VideoBackendDefinition{
            public:
                WindowingBackends windowType = window_none;
                RendererBackends rendererType = renderer_none;

                VideoBackendDefinition(WindowingBackends, RendererBackends);
                bool operator==(const VideoBackendDefinition& other)const;
                bool operator!=(const VideoBackendDefinition& other)const;
        };

        namespace VideoBackends{
            const VideoBackendDefinition none{window_none,renderer_none};

            #if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_SDL_SUPPORT)
            const VideoBackendDefinition ffmpeg_SDL2{window_sdl,renderer_ffmpeg};
            #endif
        }
    }
}
