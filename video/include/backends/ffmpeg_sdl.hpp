#pragma once

#include "generic.hpp"
#include "frame.hpp"
#include <filesystem>
#include <string>

namespace libtrainsim::backend{
    #if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_SDL_SUPPORT)
    class videoOpenFF_SDL : public videoGeneric{
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
            AVFrame * pict = nullptr;
            
            bool videoFullyLoaded = false;
            bool windowFullyCreated = false;

        public:

            /**
             * @brief Destroy the video OpenCV object
             * 
             */
            ~videoOpenFF_SDL();

            const libtrainsim::Frame getNextFrame();
            bool load(const std::filesystem::path& uri);
            void createWindow(const std::string& windowName);
            void refreshWindow();
            void displayFrame(const Frame& newFrame);
            void updateWindow();
            void gotoFrame(uint64_t frameNum);
            uint64_t getFrameCount();

    };

    #endif

}
