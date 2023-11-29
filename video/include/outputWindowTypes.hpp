#pragma once

#include "outputWindow.hpp"

#ifdef HAS_FFMPEG_SUPPORT
    #include "videoDecode/videoDecoderLibav.hpp"
#endif

#include "renderWidget/renderWidgetGL.hpp"
#include "renderWidget/renderWidgetPicture.hpp"

namespace libtrainsim::Video {
#ifdef HAS_FFMPEG_SUPPORT
    // using renderWidgetGLLibav      = renderWidgetGL<videoDecoderLibav>;
    LIBTRAINSIM_EXPORT_MACRO using renderWidgetGLLibav      = renderWidgetGL;
    LIBTRAINSIM_EXPORT_MACRO using renderWidgetPictureLibav = renderWidgetPicture<videoDecoderLibav>;

    LIBTRAINSIM_EXPORT_MACRO using outputWindow_GLLibav      = outputWindow<videoDecoderLibav, renderWidgetGLLibav>;
    LIBTRAINSIM_EXPORT_MACRO using outputWindow_PictureLibav = outputWindow<videoDecoderLibav, renderWidgetPictureLibav>;
#endif
} // namespace libtrainsim::Video