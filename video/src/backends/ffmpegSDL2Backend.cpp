#include "backends/ffmpeg_sdl.hpp"
#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::Video;
using namespace libtrainsim::backend;

#if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_SDL_SUPPORT)

videoFF_SDL::videoFF_SDL() : videoGeneric{windowSDL, rendererFF}{}

videoFF_SDL::~videoFF_SDL(){
}

#endif
