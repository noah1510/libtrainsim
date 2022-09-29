#pragma once

#include <filesystem>
#include <iostream>
#include <tuple>
#include <future>

#include "texture.hpp"
#include "helper.hpp"
#include "dimensions.hpp"

#include "base.hpp"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

namespace libtrainsim{
    namespace Video{
        class videoReader{
          private:
            /**
             * @brief the size of the video
             */
            dimensions renderSize{0,0};
            AVRational time_base;
            std::filesystem::path uri;
            
            /**
             * @brief indicates that the end of the video file is reached and the video should close.
             * This is also used to stop the video render thread.
             */
            bool reachedEOF = false;
            
            /**
             * @brief a mutex to secure access to reachedEOF and renderTimes
             */
            std::shared_mutex EOF_Mutex;
            
            /**
             * @brief all of the render times of the last render requests.
             */
            std::vector<sakurajin::unit_system::base::time_si> renderTimes;
            
            uint64_t currentFrameNumber = 0;
            uint64_t nextFrameToGet = 0;
            std::shared_mutex frameNumberMutex;

            // Private internal state
            double framerate;
            AVFormatContext* av_format_ctx;
            AVCodecContext* av_codec_ctx;
            int video_stream_index;
            AVFrame* av_frame;
            AVPacket* av_packet;
            SwsContext* sws_scaler_ctx;
            
            std::string makeAVError(int errnum);
            
            /**
             * @brief the video render thread is kept alive in this variable
             */
            std::future<bool> renderThread;
                
            /**
             * The raw pixel data of the decoded frame
             * 
             * @note this is a double buffer implementation. This way the time
             * copy operation can be done while a frame is being drawn
             */
            std::array<std::vector<uint8_t>, 2> frame_data;
            
            /**
             * @brief The index of the active buffer.
             * At the moment there are "only" two buffers so this is either 0 or 1.
             * In the future a triple Buffer might be implemented, so this could have an additional value.
             */
            uint8_t activeBuffer = 0;
            
            /**
             * @brief This keeps track if the video code has actually looked at the latest frame.
             * The buffers will only be switched if this is true.
             * This allows the Back Buffer to be updated more than once before being displayed.
             * This also prevents the front buffer being overwritten while it is read.
             */
            bool bufferExported = true;
            
            /**
             * @brief lock the access to the activeBuffer and bufferExported variable.
             * This ensures that the buffer is only switched at the correct time.
             */
            std::shared_mutex frameBuffer_mutex;
            
            void readNextFrame();
            void seekFrame(uint64_t framenumber);
            void copyToBuffer(uint8_t* frame_buffer);
            void copyToBuffer(std::vector<uint8_t>& frame_buffer);
            
        public:
            
            videoReader(const std::filesystem::path& filename);
            ~videoReader();
            
            const std::filesystem::path& getLoadedFile() const;
            bool reachedEndOfFile();
            dimensions getDimensions() const;
            uint64_t getFrameNumber();
            
            std::optional<std::vector<sakurajin::unit_system::base::time_si>> getNewRendertimes();
            void requestFrame(uint64_t frame_num);
            
            const std::vector<uint8_t>& getUsableFramebufferBuffer();
        };
    }
}
