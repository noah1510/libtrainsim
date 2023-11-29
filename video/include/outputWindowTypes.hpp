#pragma once

#include "outputWindow.hpp"

#ifdef HAS_FFMPEG_SUPPORT
    #include "videoDecode/videoDecoderLibav.hpp"
#endif

#ifdef HAS_GSTREAMER_SUPPORT
    #include "videoDecode/videoDecoderGstreamer.hpp"
#endif

#include "renderWidget/renderWidgetGL.hpp"
#include "renderWidget/renderWidgetPicture.hpp"

namespace libtrainsim::Video {
#ifdef HAS_FFMPEG_SUPPORT
    LIBTRAINSIM_EXPORT_MACRO using renderWidgetGLLibav      = renderWidgetGL<videoDecoderLibav>;
    LIBTRAINSIM_EXPORT_MACRO using renderWidgetPictureLibav = renderWidgetPicture<videoDecoderLibav>;

    LIBTRAINSIM_EXPORT_MACRO using outputWindow_GLLibav      = outputWindow<videoDecoderLibav, renderWidgetGLLibav>;
    LIBTRAINSIM_EXPORT_MACRO using outputWindow_PictureLibav = outputWindow<videoDecoderLibav, renderWidgetPictureLibav>;
#endif

#ifdef HAS_GSTREAMER_SUPPORT
    LIBTRAINSIM_EXPORT_MACRO using renderWidgetGLGstreamer      = renderWidgetGL<videoDecoderGstreamer>;
    LIBTRAINSIM_EXPORT_MACRO using renderWidgetPictureGstreamer = renderWidgetPicture<videoDecoderGstreamer>;

    LIBTRAINSIM_EXPORT_MACRO using outputWindow_GLGstreamer      = outputWindow<videoDecoderGstreamer, renderWidgetGLGstreamer>;
    LIBTRAINSIM_EXPORT_MACRO using outputWindow_PictureGstreamer = outputWindow<videoDecoderGstreamer, renderWidgetPictureGstreamer>;
#endif
} // namespace libtrainsim::Video