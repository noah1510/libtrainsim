#pragma once

#include <filesystem>
#include <iostream>
#include <tuple>
#include <future>

#include "texture.hpp"
#include "helper.hpp"
#include "dimensions.hpp"
#include "tabPage.hpp"

#include "unit_system.hpp"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/cpu.h>
}

namespace libtrainsim{
    namespace Video{
        class videoReader;
        
        /**
         * @brief the settings page for the video decoder settings
         * 
         */
        class LIBTRAINSIM_EXPORT_MACRO videoDecodeSettings : public tabPage{
          private:
            void content() override;
            videoReader& decoder;
            const std::array< std::pair<std::string, int>, 11> AlgorithmOptions;
            const std::array< std::tuple<std::string, int>, 7> AlgorithmDetailsOptions;
          public:
            videoDecodeSettings(videoReader& VR);
        };
        
        /**
         * @brief a class the handle asynchronous video decode from a single file
         * 
         * This class handles all of the video decode needs for libtrainsim.
         * Once a new object is constructed it starts a worker thread which does the actual decoding.
         * Outside of this class a new frame can be requested from the video decoder. However that new
         * requested frame must be further in the video than the currently displayed one.
         * 
         * @note if you want to check if the video decoder has quit use the reachedEndOfFile function.
         * 
         */
        class LIBTRAINSIM_EXPORT_MACRO videoReader{
          friend class videoDecodeSettings;
          private:
              
            // all of the ffmpeg state variables
            
            //The framerate of the video as double
            double framerate;
            //the avcontext
            AVFormatContext* av_format_ctx = NULL;
            //the av codec context
            AVCodecContext* av_codec_ctx = NULL;
            //the id of the video stream
            int video_stream_index;
            //The most recently decoded frame
            AVFrame* av_frame = NULL;
            //the most recent packet
            AVPacket* av_packet = NULL;
            //the SwsContext for scaling and color space conversion
            SwsContext* sws_scaler_ctx = NULL;
            
            //signal if hardware decoding should be used
            bool enableHWDecode = false;
            //The Hardware decoding context
            AVBufferRef* hw_device_ctx = NULL;
            //The most recently accelerator decoded frame
            AVFrame* hw_av_frame = NULL;
            //A list with which hardware decode backend to prefer
            const std::array<AVHWDeviceType, 11> hardwareBackendPrioList = {
                AV_HWDEVICE_TYPE_VDPAU,
                AV_HWDEVICE_TYPE_CUDA,
                AV_HWDEVICE_TYPE_VAAPI,
                AV_HWDEVICE_TYPE_DXVA2,
                AV_HWDEVICE_TYPE_VULKAN,
                AV_HWDEVICE_TYPE_QSV,
                AV_HWDEVICE_TYPE_VIDEOTOOLBOX,
                AV_HWDEVICE_TYPE_D3D11VA,
                AV_HWDEVICE_TYPE_DRM,
                AV_HWDEVICE_TYPE_OPENCL,
                AV_HWDEVICE_TYPE_MEDIACODEC
            };
            
            //The params for the scaling context
            int scalingContextParams = SWS_SINC;
            
            //a mutex for locking the scaling context parameter
            std::shared_mutex contextMutex;
            
            /**
             * @brief the size of the video
             */
            dimensions renderSize{0,0};
            
            /**
             * @brief the path to the video file
             */
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
            std::vector<sakurajin::unit_system::time_si> renderTimes;
            
            /**
             * @brief the number of the currently displayed frame
             */
            uint64_t currentFrameNumber = 0;
            
            /**
             * @brief the number of the next frame that should be read.
             */
            uint64_t nextFrameToGet = 0;
            
            /**
             * @brief a mutex for currentFrameNumber and nextFrameToGet
             * This mutex controls the access to the frame id variables.
             * Inderectly this also controls the access to the ffmpeg state variables
             */
            std::shared_mutex frameNumberMutex;
            
            /**
             * @brief the video render thread is kept alive in this variable
             */
            std::future<bool> renderThread;
            
            /**
             * @brief the number of frame data buffers.
             * This has to be at least 2 and the default will be 3
             */
            static const size_t FRAME_BUFFER_COUNT = 3;
                
            /**
             * The raw pixel data of the decoded frame
             * 
             * @note this is a double buffer implementation. This way the time
             * copy operation can be done while a frame is being drawn
             */
            std::array<std::vector<uint8_t>, FRAME_BUFFER_COUNT> frame_data;
            
            /**
             * @brief The index of the active buffer.
             * By defualt there are 3 buffers.
             * To make sure this variable always has a valid value only use
             * incrementFramebuffer to switch between the buffers.
             */
            uint8_t activeBuffer = 0;
            
            /**
             * @brief This keeps track if the video code has actually looked at the latest frame.
             * The buffers will only be switched if this is true.
             * This allows the Back Buffer to be updated more than once before being displayed.
             * This also prevents the front buffer being overwritten while it is read.
             */
            bool bufferExported = false;
            
            /**
             * @brief lock the access to the activeBuffer and bufferExported variable.
             * This ensures that the buffer is only switched at the correct time.
             * Because the Buffer ids are mutex protected and the read/write from/to the
             * frame_data is all regulated in this class, the frame_data access does not
             * need to be locked directly.
             */
            std::shared_mutex frameBuffer_mutex;
            
            /**
             * @brief increment a framebuffer number to the next buffer in line
             * 
             * This increments a given buffer number by one and makes sure it stays
             * one of the valid buffers.
             */
            inline void incrementFramebuffer(uint8_t& currentBuffer) const;
            
            /**
             * @brief reads the next frame in the video file into av_frame.
             * @note this function does not update the currentFrameNumber variable
             */
            void readNextFrame();
            
            /**
             * @brief jump directly to a given frame number
             * @todo fix seek for non const framerate (at the moment the fram number is
             * converted to a timestamp based on the assumption that the framerate is constant).
             * @param framenumber the number of the frame the decode should seek.
             */
            void seekFrame(uint64_t framenumber);
            
            /**
             * @brief copy the av_frame to the given frame_buffer
             * 
             * This function uses swscale to copy the frame data into a buffer and
             * while doing that converts the frame to rgba8888.
             * 
             * @param frame_buffer The frame buffer the frame data should be copied into
             */
            void copyToBuffer(std::vector<uint8_t>& frame_buffer);
            
            /**
             * @brief how many frames difference there has to be to seek instead of rendering frame by frame.
             * 
             */
            uint64_t seekCutoff = 200;
            
            void initHWDecoding(AVCodec* av_codec);
        public:
            /**
             * @brief create a new video decoder for a given video file
             * 
             * @param filename the path to the file that should be played back by this object
             * @param threadCount the number of threads that should be used for video decode, 0 for autodetect
             */
            videoReader(const std::filesystem::path& filename, uint64_t threadCount = 0);
            
            /**
             * @brief destroys the video decoder
             */
            ~videoReader();
            
            /**
             * @brief request a frame to be decoded as soon as possible
             * 
             * This updates the frame number of the next frame that will be decoded
             * by the worker thread.
             * 
             * @note if the given frame number is smaller than an already requested frame it will be ignored.
             */
            void requestFrame(uint64_t frame_num);
            
            /**
             * @brief get a reference to the currently active framebuffer.
             * 
             * This is the function that need to be called if you want to update the
             * output of the program.
             * 
             * @note this might not be the latest requested frame since that might be in the back buffer.
             */
            const std::vector<uint8_t>& getUsableFramebufferBuffer();
            
            /**
             * @brief get the path of the video that is being played back
             * @return the path to the played back video file
             */
            const std::filesystem::path& getLoadedFile() const;
            
            /**
             * @brief check if the decoder has reached the end of the video file or encountered an error
             * 
             * If EOF has been reached or an error happened this function will return true.
             * Once this function returns true the worker thread will end and no longer be able
             * to precess further requests for frames.
             */
            bool reachedEndOfFile();
            
            /**
             * @brief get the dimensions of the video file
             */
            dimensions getDimensions() const;
            
            /**
             * @brief get the currently displayed frame number
             * @note depending on the internal state this may be the frame number of the active buffer or the back buffer.
             * 
             */
            uint64_t getFrameNumber();
            
            /**
             * @brief get the times the worker needed to decode its decode requests
             * if not new frame was decoded the return has no value.
             */
            std::optional<std::vector<sakurajin::unit_system::time_si>> getNewRendertimes();
        };
    }
}
