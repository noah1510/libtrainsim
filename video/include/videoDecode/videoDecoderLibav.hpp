#pragma once

#include "videoDecoderBase.hpp"

namespace libtrainsim {
    namespace Video {
        class videoDecoderLibav;

        /**
         * @brief the settings page for the video decoder settings
         *
         */
        /*class LIBTRAINSIM_EXPORT_MACRO videoDecodeSettings : public SimpleGFX::SimpleGL::tabPage{
          private:
            void content() override;
            videoDecoderLibav& decoder;
            const std::array< std::pair<std::string, int>, 11> AlgorithmOptions;
            const std::array< std::tuple<std::string, int>, 7> AlgorithmDetailsOptions;
          public:
            videoDecodeSettings(videoDecoderLibav& VR);
        };*/

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
        class LIBTRAINSIM_EXPORT_MACRO videoDecoderLibav : public videoDecoderBase {
            // friend class videoDecodeSettings;
          private:
            // all the ffmpeg state variables

            // The framerate of the video as double
            double framerate;
            // the avcontext
            AVFormatContext* av_format_ctx = nullptr;
            // the av codec context
            AVCodecContext* av_codec_ctx = nullptr;
            // the id of the video stream
            int video_stream_index;
            // The most recently decoded frame
            AVFrame* av_frame = nullptr;
            // the most recent packet
            AVPacket* av_packet = nullptr;
            // the SwsContext for scaling and color space conversion
            SwsContext* sws_scaler_ctx = nullptr;

            // The params for the scaling context
            int scalingContextParams = SWS_SINC;

            // a mutex for locking the scaling context parameter
            std::shared_mutex contextMutex;


          protected:

            /**
             * @brief reads the next frame in the video file into av_frame.
             * @note this function does not update the currentFrameNumber variable
             */
            void readNextFrame() override;

            /**
             * @brief jump directly to a given frame number
             * @todo fix seek for non const framerate (at the moment the fram number is
             * converted to a timestamp based on the assumption that the framerate is constant).
             * @param framenumber the number of the frame the decode should seek.
             */
            void seekFrame(uint64_t framenumber) override;

            /**
             * @brief copy the av_frame to the given frame_buffer
             *
             * This function uses swscale to copy the frame data into a buffer and
             * while doing that converts the frame to rgba8888.
             *
             * @param frame_buffer The frame buffer the frame data should be copied into
             */
            void copyToBuffer(std::vector<uint8_t>& frame_buffer) override;

          public:
            /**
             * @brief create a new video decoder for a given video file
             *
             * @param filename the path to the file that should be played back by this object
             * @param threadCount the number of threads that should be used for video decode, 0 for autodetect
             */
            explicit videoDecoderLibav(std::filesystem::path videoFile, std::shared_ptr<SimpleGFX::logger> _logger, uint64_t _seekCutoff = 200, uint64_t threadCount = 0);

            /**
             * @brief destroys the video decoder
             */
            ~videoDecoderLibav();
        };
    } // namespace Video
} // namespace libtrainsim
