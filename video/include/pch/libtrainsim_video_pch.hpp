#include "core.hpp"

#include "simplegl.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/cpu.h>
#include <libswscale/swscale.h>
}