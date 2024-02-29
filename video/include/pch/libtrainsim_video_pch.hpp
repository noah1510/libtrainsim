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
    #include <gst/video/video.h>
}
#endif

#ifdef HAS_VLC_SUPPORT
#include "vlcpp/vlc.hpp"

#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
#define LIBTRAINSIM_VLC4_MODE
#else
#define LIBTRAINSIM_VLC3_MODE
#endif

#endif

#include <type_traits>
