#pragma once

#include "generic.hpp"
#include "frame.hpp"
#include <filesystem>
#include <string>

namespace libtrainsim{
    namespace backend{
        #if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_SDL_SUPPORT)
        class videoFF_SDL : public videoGeneric{
            private:
                AVFormatContext* pFormatCtx = nullptr;
                AVCodecContext* pCodecCtx = nullptr;
                AVCodec* pCodec = nullptr;
                int videoStream = -1;
                
                SDL_Window* screen = nullptr;
                SDL_Renderer* renderer = nullptr;
                SDL_Texture* texture = nullptr;
                
                int numBytes = 0;
                struct SwsContext* sws_ctx = nullptr;
                AVPacket* pPacket = nullptr;
                uint8_t * buffer = nullptr;
                
                bool videoFullyLoaded = false;
                bool windowFullyCreated = false;
                
                Frame pict;

            protected:
                void displayFrame(const Frame& newFrame);
                const libtrainsim::Frame getNextFrame();
            public:

                /**
                * @brief Destroy the video FF_SDL object
                * 
                */
                ~videoFF_SDL();

                //The functions below are just reimplemented from video_generic

                bool load(const std::filesystem::path& uri);
                void createWindow(const std::string& windowName);
                void refreshWindow();
                void gotoFrame(uint64_t frameNum);
                uint64_t getFrameCount();
                double getHight();
                double getWidth();

        };

        #endif

    }
}
