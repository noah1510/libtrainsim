#include "backends/ffmpeg_glfw3.hpp"
#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::Video;

#if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_GLFW_SUPPORT)

videoFF_glfw3::videoFF_glfw3() : videoGeneric{windowglfw, rendererFF}{}

videoFF_glfw3::~videoFF_glfw3(){
}

GLFWwindow* videoFF_glfw3::getGLFWwindow(){
    return windowglfw.getGLFWwindow();
}

#endif

