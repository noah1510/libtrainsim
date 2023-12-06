#pragma once

#include "videoDecoderBase.hpp"

namespace libtrainsim {
    namespace Video {
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
        class LIBTRAINSIM_EXPORT_MACRO videoDecoderGstreamer : public videoDecoderBase {
          private:
            GstElement* pipeline = nullptr;
            GstElement* source   = nullptr;
            GstElement* videorate = nullptr;
            GstElement* convert = nullptr;
            GstElement* sink    = nullptr;

            unsigned int      bus_watch_id = 0;
            std::atomic<bool> isStepping   = false;
            const bool        logTags      = false;

            void        pad_added_handler(GstElement* src, GstPad* new_pad);
            static void pad_added_callback_connector(GstElement* src, GstPad* new_pad, videoDecoderGstreamer* self) {
                self->pad_added_handler(src, new_pad);
            }


            bool       handleMessages(GstBus* bus, GstMessage* message);
            static int handleMessages_connector(GstBus* bus, GstMessage* message, gpointer data) {
                return static_cast<videoDecoderGstreamer*>(data)->handleMessages(bus, message);
            }

          protected:
            /**
             * @brief reads the next frame in the video file into av_frame.
             * @note this function does not update the currentFrameNumber variable
             */
            // void readNextFrame() override;

            /**
             * @brief jump directly to a given frame number
             * @todo fix seek for non const framerate (at the moment the fram number is
             * converted to a timestamp based on the assumption that the framerate is constant).
             * @param framenumber the number of the frame the decode should seek.
             */
            // void seekFrame(uint64_t framenumber) override;

            /**
             * @brief copy the av_frame to the given frame_buffer
             *
             * This function uses swscale to copy the frame data into a buffer and
             * while doing that converts the frame to rgba8888.
             *
             * @param frame_buffer The frame buffer the frame data should be copied into
             */
            void fillInternalPixbuf(std::shared_ptr<Gdk::Pixbuf>& pixbuf) override;

            bool renderLoop() override;

          public:
            /**
             * @brief create a new video decoder for a given video file
             *
             * @param filename the path to the file that should be played back by this object
             * @param threadCount the number of threads that should be used for video decode, 0 for autodetect
             */
            explicit videoDecoderGstreamer(std::filesystem::path              videoFile,
                                           std::shared_ptr<SimpleGFX::logger> _logger,
                                           uint64_t                           _seekCutoff = 200,
                                           uint64_t                           threadCount = 0);

            /**
             * @brief destroys the video decoder
             */
            ~videoDecoderGstreamer();
        };
    } // namespace Video
} // namespace libtrainsim
