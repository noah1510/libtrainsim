#pragma once

#include "frame.hpp"

#include <filesystem>


namespace libtrainsim{
    namespace Video{
        class videoDecoder{
        private:
            
            AVFormatContext* pFormatCtx = nullptr;
            AVCodecContext* pCodecCtx = nullptr;
            AVCodec* pCodec = nullptr;
            int videoStream = -1;
            
            int numBytes = 0;
            struct SwsContext* sws_ctx = nullptr;
            AVPacket* pPacket = nullptr;
            uint8_t * buffer = nullptr;
            
             /**
            * @brief This is that path to the currently loaded file.
            *
            */
            std::filesystem::path loadedFile;

            /**
            * @brief The number of the frame that is currently displayed
            */
            uint64_t currentFrameNumber = 0;
            
            /**
             * @brief This is true when the end of the video was reached.
             * To be safe this is true by default.
             */
            bool endOfFile = true;
        public:
            /**
            * @brief Destroy the generic renderer object
            *
            */
            ~videoDecoder();
            
            /**
            * @brief Load a video file into the video management.
            *
            * @param uri The uri of the file.
            * @return true file sucessfully loaded
            * @return false error while loading file
            */
            bool load(const std::filesystem::path& uri);
            
            /**
             * @brief jump to the given frame number and return that frame
             * 
             * @param frameNum the frame you want ro jump to
             * @return std::shared_ptr<libtrainsim::Frame> the frame at the wanted location
             */
            std::shared_ptr<libtrainsim::Video::Frame> gotoFrame(uint64_t frameNum);

            /**
            * @brief get the total number of frames for the loaded video file
            * @return the total number of frames
            *
            */
            uint64_t getFrameCount();

            /**
             * @brief Gget the number of the currently displayed frame
             * 
             * @return uint64_t the number of the current frame
             */
            uint64_t getCurrentFrameNumber();
            
            /**
            * @brief Get the Height of the video in pixels
            *
            * @return double
            */
            double getHeight();

            /**
            * @brief Get the Width of the video in pixels
            *
            * @return double
            */
            double getWidth();

            /**
            * @brief get the currently loaded File.
            *
            */
            const std::filesystem::path& getLoadedFile() const;
            
            /**
            * @brief Retrieve the next frame to display it.
            * If no video is loaded or there is no new frame, an empty frame will be returned.
            * You should check the returned frame with the method .empty(), which will return true if the frame is empty.
            *
            * @return std::shared_ptr<libtrainsim::Video::Frame> The next frame of the video
            */
            std::shared_ptr<libtrainsim::Video::Frame> getNextFrame();
            
            /**
             * @brief scale a frame to the required buffer size and color format.
             * @note the passed data will no be modified and is read only. The scaled data is in the return value.
             * @param frame The frame data that should be scaled
             * @return std::shared_ptr<libtrainsim::Video::Frame> the scaled frame data
             */
            std::shared_ptr<libtrainsim::Video::Frame> scaleFrame(std::shared_ptr<libtrainsim::Video::Frame> frame);
            
            /**
             * @brief initiate an empty frame.
             * This sets the correct backend and copies the last frame into it.
             * @param frame The frame that should be initiated.
             */
            void initFrame(std::shared_ptr<libtrainsim::Video::Frame> frame);
            
            /**
             * @brief check if the end of a video file is reached.
             * Use this function to stop the simulator if the video file is over.
             * The simulator will hang or crash once the end is reached and it still tries to render new frames.
             * 
             * @return true The end of the video file is reached
             * @return false The video file is not at the end yet
             */
            bool reachedEndOfFile();
        };
    }
}
