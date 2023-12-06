#pragma once

#ifndef LIBTRAINSIM_EXPORT_MACRO
    #define LIBTRAINSIM_EXPORT_MACRO
#endif

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
        class LIBTRAINSIM_EXPORT_MACRO videoDecoderBase : public sigc::trackable {
          private:
            bool renderLoopCaller();
          protected:
            /**
             * @brief the size of the video
             */
            SimpleGFX::SimpleGL::dimensions renderSize{0, 0};

            /**
             * @brief the path to the video file
             */
            std::filesystem::path uri;

            /**
             * @brief indicates that the end of the video file is reached and the video should close.
             * This is also used to stop the video render thread.
             */
            std::atomic<bool> reachedEOF = false;

            /**
             * @brief all of the render times of the last render requests.
             */
            std::vector<sakurajin::unit_system::time_si> renderTimes;

            /**
             * @brief A mutex to secure the access to the rendertimes array
             */
            std::shared_mutex renderTimeMutex;

            /**
             * @brief the number of the currently displayed frame
             */
            std::atomic<uint64_t> currentFrameNumber = 0;

            /**
             * @brief the number of the next frame that should be read.
             */
            std::atomic<uint64_t> nextFrameToGet = 1;

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
            std::array<std::shared_ptr<Gdk::Pixbuf>, FRAME_BUFFER_COUNT> frame_data;

            /**
             * @brief The index of the active buffer.
             * By defualt there are 3 buffers.
             * To make sure this variable always has a valid value only use
             * incrementFramebuffer to switch between the buffers.
             */
            std::atomic<uint8_t> activeBuffer = 0;

            /**
             * @brief This keeps track if the video code has actually looked at the latest frame.
             * The buffers will only be switched if this is true.
             * This allows the Back Buffer to be updated more than once before being displayed.
             * This also prevents the front buffer being overwritten while it is read.
             */
            std::atomic<bool> bufferExported = false;

            /**
             * @brief increment a framebuffer number to the next buffer in line
             *
             * This increments a given buffer number by one and makes sure it stays
             * one of the valid buffers.
             */
            [[nodiscard]]
            uint8_t incrementFramebuffer(uint8_t currentBuffer) const;

            /**
             * @brief how many frames difference there has to be to seek instead of rendering frame by frame.
             *
             */
            std::atomic<uint64_t> seekCutoff;

            /**
             * @brief A pointer to the used loggin interface
             */
            std::shared_ptr<SimpleGFX::logger> LOGGER = nullptr;

            void startRendering();

            /**
             * @brief create a new video decoder for a given video file
             *
             * @param filename the path to the file that should be played back by this object
             * @param threadCount the number of threads that should be used for video decode, 0 for autodetect
             */
            explicit videoDecoderBase(std::filesystem::path              videoFile,
                                      std::shared_ptr<SimpleGFX::logger> _logger,
                                      uint64_t                           _seekCutoff = 200);

            /**
             * @brief reads the next frame in the video file into av_frame.
             * @note this function does not update the currentFrameNumber variable
             */
            virtual void readNextFrame();

            /**
             * @brief jump directly to a given frame number
             * @todo fix seek for non const framerate (at the moment the fram number is
             * converted to a timestamp based on the assumption that the framerate is constant).
             * @param framenumber the number of the frame the decode should seek.
             */
            virtual void seekFrame(uint64_t framenumber);

            /**
             * @brief copy the av_frame to the given frame_buffer
             *
             * This function uses swscale to copy the frame data into a buffer and
             * while doing that converts the frame to rgba8888.
             *
             * @param frame_buffer The frame buffer the frame data should be copied into
             */
            virtual void copyToBuffer(std::vector<uint8_t>& frame_buffer);

            virtual void fillInternalPixbuf(std::shared_ptr<Gdk::Pixbuf>& pixbuf);

            /**
             * @brief the main render loop of the video decoder
             * @return true if the render loop exited without error false if something went wrong
             */
            virtual bool renderLoop();

            /**
             * @brief render until the most recent frame is reached
             * @return true if the rendering was successful false if an error happened
             */
            bool renderRequestedFrame();

          public:
            /**
             * @brief destroys the video decoder
             */
            ~videoDecoderBase();

            /**
             * @brief request a frame to be decoded as soon as possible
             *
             * This updates the frame number of the next frame that will be decoded
             * by the worker thread.
             *
             * @note if the given frame number is smaller than an already requested frame it will be ignored.
             */
            [[maybe_unused]]
            bool requestFrame(uint64_t frame_num);

            /**
             * @brief get the usable framebuffer as pixbuf
             *
             * This function performs an internal conversion from the framebuffer to a pixbuf.
             * The returned object can be used to display the image in a gtk widget.
             * @param pixbuf the pixbuf that should be used to store the framebuffer nullptr to create a new pixbuf
             * @return The usable framebuffer as pixbuf
             */
            [[maybe_unused]] [[nodiscard]]
            std::shared_ptr<Gdk::Pixbuf> getUsablePixbuf(std::shared_ptr<Gdk::Pixbuf> pixbuf = nullptr);

            /**
             * Returns true if a new frame is available using getUsableFramebuffer
             * @return true if a new frame is available false otherwise
             */
            [[nodiscard]]
            bool hasNewPixbuf();

            /**
             * Returns true if a new frame is available using getUsableFramebuffer
             * @return true if a new frame is available false otherwise
             */
            [[nodiscard]] [[deprecated("use hasNewPixbuf instead")]]
            bool hasNewFramebuffer();

            /**
             * @brief get the path of the video that is being played back
             * @return the path to the played back video file
             */
            [[maybe_unused]] [[nodiscard]]
            const std::filesystem::path& getLoadedFile() const;

            /**
             * @brief check if the decoder has reached the end of the video file or encountered an error
             *
             * If EOF has been reached or an error happened this function will return true.
             * Once this function returns true the worker thread will end and no longer be able
             * to precess further requests for frames.
             */
            [[nodiscard]]
            bool reachedEndOfFile() const;

            /**
             * @brief get the dimensions of the video file
             */
            [[maybe_unused]] [[nodiscard]]
            SimpleGFX::SimpleGL::dimensions getDimensions() const;

            /**
             * @brief get the currently displayed frame number
             * @note depending on the internal state this may be the frame number of the active buffer or the back buffer.
             *
             */
            [[maybe_unused]] [[nodiscard]]
            uint64_t getFrameNumber();

            /**
             * @brief get the times the worker needed to decode its decode requests
             * if not new frame was decoded the return has no value.
             */
            [[maybe_unused]] [[nodiscard]]
            std::optional<std::vector<sakurajin::unit_system::time_si>> getNewRendertimes();
        };

        template<class decoderClass>
        concept videoDecoderClass = std::is_base_of<::libtrainsim::Video::videoDecoderBase, decoderClass>::value;
    } // namespace Video
} // namespace libtrainsim
