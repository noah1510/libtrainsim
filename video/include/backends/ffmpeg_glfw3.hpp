#pragma once

#include "genericBackend.hpp"
#include "frame.hpp"
#include "ffmpegRenderer.hpp"
#include "glfw3WindowManager.hpp"

#include <filesystem>
#include <string>

namespace libtrainsim{
    namespace Video{
        #if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_GLFW_SUPPORT)
        
        class videoFF_glfw3 : public videoGeneric{
            private:
                ffmpegRenderer rendererFF{};
                
                glfw3WindowManager windowglfw{rendererFF};

            public:

                videoFF_glfw3();
                
                /**
                * @brief Destroy the video FF_SDL object
                * 
                */
                ~videoFF_glfw3();

                GLFWwindow* getGLFWwindow();
        };

        #endif

    }
}

