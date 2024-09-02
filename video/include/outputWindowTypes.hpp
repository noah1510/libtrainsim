#pragma once

#include "outputWindow.hpp"

#include "renderWidget/renderWidgetGL.hpp"
#include "renderWidget/renderWidgetPicture.hpp"


#define MAKE_WINDGET_CONSTRUCTOR(CLASSNAME, PARENT_CLASS)       \
    template <typename... decoderArgs> [[maybe_unused]]         \
    explicit CLASSNAME(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings, std::shared_ptr<SimpleGFX::SimpleGL::appLauncher> _mainAppLauncher, decoderArgs&&... _args)     \
        : PARENT_CLASS{std::move(_simSettings), std::move(_mainAppLauncher), &_args...}{};

#define MAKE_RENDER_WIDGET_CLASS(CLASSNAME, PARENT_CLASS, DECODE_CLASS)             \
    LIBTRAINSIM_EXPORT_MACRO class CLASSNAME : public PARENT_CLASS<DECODE_CLASS>{   \
      public: MAKE_WINDGET_CONSTRUCTOR(CLASSNAME, PARENT_CLASS)                     \
    };

#define MAKE_OUTPUT_WINDOW_CLASS(CLASSNAME, RENDER_WIDGET_CLASS, DECODE_CLASS)                          \
    LIBTRAINSIM_EXPORT_MACRO class CLASSNAME : public outputWindow<DECODE_CLASS, RENDER_WIDGET_CLASS> { \
      public: MAKE_WINDGET_CONSTRUCTOR(CLASSNAME, outputWindow)                                         \
    };

#define MAKE_CLASSES_FOR_BACKEND(BACKEND_SHORTNAME, DECODE_CLASS)                                                                   \
    namespace libtrainsim::Video {                                                                                                  \
        MAKE_RENDER_WIDGET_CLASS(renderWidgetGL ## BACKEND_SHORTNAME, renderWidgetGL, DECODE_CLASS)                                 \
        MAKE_OUTPUT_WINDOW_CLASS(outputWindow_GL ## BACKEND_SHORTNAME, renderWidgetGL ## BACKEND_SHORTNAME, DECODE_CLASS)           \
                                                                                                                                    \
        MAKE_RENDER_WIDGET_CLASS(renderWidgetPicture ## BACKEND_SHORTNAME, renderWidgetPicture, DECODE_CLASS)                       \
        MAKE_OUTPUT_WINDOW_CLASS(outputWindow_Picture ## BACKEND_SHORTNAME, renderWidgetPicture ## BACKEND_SHORTNAME, DECODE_CLASS) \
    }


#ifdef HAS_FFMPEG_SUPPORT
    #include "videoDecode/videoDecoderLibav.hpp"
    MAKE_CLASSES_FOR_BACKEND(Libav, videoDecoderLibav)
#endif

#ifdef HAS_GSTREAMER_SUPPORT
    #include "videoDecode/videoDecoderGstreamer.hpp"
    MAKE_CLASSES_FOR_BACKEND(Gstreamer, videoDecoderGstreamer)
#endif

#ifdef HAS_VLC_SUPPORT
    #include "videoDecode/videoDecoderVlc.hpp"
    MAKE_CLASSES_FOR_BACKEND(Vlc, videoDecoderVlc)
#endif
