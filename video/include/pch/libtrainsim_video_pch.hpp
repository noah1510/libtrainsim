#include "core.hpp"

#include "simplegl.hpp"

#include "video_config.hpp"

#ifdef HAS_FFMPEG_SUPPORT
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/cpu.h>
    #include <libswscale/swscale.h>
}
#endif

#ifdef HAS_GSTREAMER_SUPPORT
extern "C" {
    #include <gst/gst.h>
}
#endif

#include <type_traits>
