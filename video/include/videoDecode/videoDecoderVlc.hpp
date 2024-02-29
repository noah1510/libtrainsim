#pragma once

#include "videoDecoderBase.hpp"

namespace libtrainsim::Video {

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
    class LIBTRAINSIM_EXPORT_MACRO videoDecoderVlc : public videoDecoderBase {
        // friend class videoDecodeSettings;
      private:
        // all the libvlc stuff
        std::unique_ptr<VLC::MediaPlayer> player = nullptr;
        uint32_t fps_num = 0;
        uint32_t fps_den = 0;

        int videoTrackID = -1;

        // the actual init function
        void initVlc();

        // the render surface
        std::shared_ptr<Gdk::Pixbuf> renderSurface;
        std::shared_mutex            renderSurfaceMutex;

        // The callbacks to render into the surface instead of windows
        void* lockBuffer(void** p_pixels);
        void unlockBuffer(void* id, void* const* p_pixels);
        void displayBuffer(void* id);

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
        void copyToBuffer(std::shared_ptr<Gdk::Pixbuf>& pixbuf) override;

        //bool renderLoop() override;

      public:
        /**
         * @brief create a new video decoder for a given video file
         *
         * @param filename the path to the file that should be played back by this object
         * @param threadCount the number of threads that should be used for video decode, 0 for autodetect
         */
        explicit videoDecoderVlc(std::filesystem::path              videoFile,
                                 std::shared_ptr<SimpleGFX::logger> _logger,
                                 uint64_t                           _seekCutoff = 200,
                                 uint64_t                           threadCount = 0);

        /**
         * @brief destroys the video decoder
         */
        ~videoDecoderVlc();
    };
} // namespace libtrainsim::Video
