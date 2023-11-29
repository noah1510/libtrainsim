#pragma once

//this is the header used to include the libtrainsim video module
#include "pch/libtrainsim_video_pch.hpp"

#include "videoDecode/videoDecoderBase.hpp"

#ifdef HAS_FFMPEG_SUPPORT
    #include "videoDecode/videoDecoderLibav.hpp"
#endif

#include "renderWidget/renderWidgetBase.hpp"
#include "renderWidget/renderWidgetGL.hpp"
#include "renderWidget/renderWidgetPicture.hpp"

#include "outputWindow.hpp"
#include "outputWindowTypes.hpp"
