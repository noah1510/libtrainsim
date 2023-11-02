// libtrainsim core
#include "core.hpp"

// include video if that is enabled
#ifdef HAS_VIDEO_SUPPORT
    #if __has_include("video.hpp")
        #include "video.hpp"
    #else
        #undef HAS_VIDEO_SUPPORT
    #endif
#endif

// external libraries
#include "eventSystem.hpp"
#include "rs232.hpp"
