#include "backends/ffmpeg_sdl.hpp"
#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::Video;

#if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_SDL_SUPPORT)

videoFF_SDL::videoFF_SDL() : videoGeneric{}{
    renderer = std::make_shared<ffmpegRenderer>();
    window = std::make_shared<SDL2WindowManager>(renderer);
}

videoFF_SDL::~videoFF_SDL(){
}

#endif
