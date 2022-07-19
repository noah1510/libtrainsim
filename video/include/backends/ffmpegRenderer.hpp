#pragma once

#include "genericRenderer.hpp"
#include "backends/ffmpegFrame.hpp"
#include <filesystem>
#include <string>

namespace libtrainsim {
    namespace Video{
        #if defined(HAS_FFMPEG_SUPPORT)
        
            class ffmpegRenderer : public genericRenderer{
            private:
                AVFormatContext* pFormatCtx = nullptr;
                AVCodecContext* pCodecCtx = nullptr;
                AVCodec* pCodec = nullptr;
                int videoStream = -1;
                
                int numBytes = 0;
                struct SwsContext* sws_ctx = nullptr;
                AVPacket* pPacket = nullptr;
                uint8_t * buffer = nullptr;
                
            protected:
                
            public:
                
                /**
                * @brief Destroy the ffmpeg renderer object
                * 
                */
                ~ffmpegRenderer();

                //The functions below are just reimplemented from generic renderer

                bool load(const std::filesystem::path& uri);
                std::shared_ptr<libtrainsim::Frame> gotoFrame(uint64_t frameNum);
                uint64_t getFrameCount();
                double getHeight();
                double getWidth();
                std::shared_ptr<libtrainsim::Frame> getNextFrame();
                std::shared_ptr<libtrainsim::Frame> scaleFrame(std::shared_ptr<libtrainsim::Frame> frame);
                void initFrame(std::shared_ptr<libtrainsim::Frame> frame);
            };
        
        #endif
    }
}
