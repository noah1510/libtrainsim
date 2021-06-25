#pragma once

#include "genericRenderer.hpp"
#include "frame.hpp"
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
                * @brief Destroy the video FF_SDL object
                * 
                */
                ~ffmpegRenderer();

                //The functions below are just reimplemented from video_generic

                bool load(const std::filesystem::path& uri);
                const libtrainsim::Frame gotoFrame(uint64_t frameNum);
                uint64_t getFrameCount();
                double getHight();
                double getWidth();
                
                /**
                * @brief Retrieve the next frame to display it.
                * If no video is loaded or there is no new frame, an empty frame will be returned.
                * You should check the returned frame with the method .empty(), which will return true if the frame is empty.
                *
                * @return const libtrainsim::Frame The next frame of the video
                */
                const libtrainsim::Frame getNextFrame();
                
                libtrainsim::Frame scaleFrame(const libtrainsim::Frame& frame);
                void initFrame(libtrainsim::Frame& frame);
            };
        
        #endif
    }
}
