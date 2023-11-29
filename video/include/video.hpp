#pragma once

//this is the header used to include the libtrainsim video module
#include "pch/libtrainsim_video_pch.hpp"

#include "videoDecode/videoDecoderBase.hpp"

#include "renderWidget/renderWidgetBase.hpp"
#include "renderWidget/renderWidgetGL.hpp"
#include "renderWidget/renderWidgetPicture.hpp"

#include "outputWindow.hpp"

//the window types header includes the video decoders that are available
//It defines template specializations for the output window class
#include "outputWindowTypes.hpp"
