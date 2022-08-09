#ifndef video_reader_hpp
#define video_reader_hpp

#include <filesystem>
#include <iostream>
#include <tuple>
#include "texture.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}

namespace libtrainsim{
    namespace Video{
        class videoReader{
          private:
            int width, height;
            AVRational time_base;
            std::filesystem::path uri;
            bool reachedEOF = false;
            uint64_t currentFrameNumber = 0;

            // Private internal state
            AVFormatContext* av_format_ctx;
            AVCodecContext* av_codec_ctx;
            int video_stream_index;
            AVFrame* av_frame;
            AVPacket* av_packet;
            SwsContext* sws_scaler_ctx;
            
            std::string makeAVError(int errnum);
            
        public:
            
            videoReader(const std::filesystem::path& filename);
            ~videoReader();
            void readNextFrame();
            void seekFrame(uint64_t framenumber);
            void copyToBuffer(uint8_t* frame_buffer);
            void copyToBuffer(std::vector<uint8_t>& frame_buffer);
            
            const std::filesystem::path& getLoadedFile() const;
            bool reachedEndOfFile() const;
            dimensions getDimensions() const;
            uint64_t getFrameNumber() const;
        };
    }
}

#endif
