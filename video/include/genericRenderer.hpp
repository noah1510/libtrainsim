#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include "frame.hpp"

namespace libtrainsim {
    namespace Video{
        /**
         * @brief This is a generic video renderer.
         * The video renderer loads a video file and decodes the video.
         * Using getNextFrame a new frame can be retreived from the video and
         * can be displayed on any window.
         *
         * This class mostly defines which functions each renderer needs to have.
         * All of the virtual functions need to be reimplemented in the child classes.
         */
        class genericRenderer{
        protected:
            
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
            virtual ~genericRenderer();
            
            /**
            * @brief Load a video file into the video management.
            *
            * @param uri The uri of the file.
            * @return true file sucessfully loaded
            * @return false error while loading file
            */
            virtual bool load(const std::filesystem::path& uri);
            
            /**
             * @brief jump to the given frame number and return that frame
             * 
             * @param frameNum the frame you want ro jump to
             * @return std::shared_ptr<libtrainsim::Frame> the frame at the wanted location
             */
            virtual std::shared_ptr<libtrainsim::Frame> gotoFrame(uint64_t frameNum);

            /**
            * @brief get the total number of frames for the loaded video file
            * @return the total number of frames
            *
            */
            virtual uint64_t getFrameCount();

            /**
             * @brief Gget the number of the currently displayed frame
             * 
             * @return uint64_t the number of the current frame
             */
            virtual uint64_t getCurrentFrameNumber();
            
            /**
            * @brief Get the Height of the video in pixels
            *
            * @return double
            */
            virtual double getHeight();

            /**
            * @brief Get the Width of the video in pixels
            *
            * @return double
            */
            virtual double getWidth();

            /**
            * @brief get the currently loaded File.
            *
            */
            virtual const std::filesystem::path& getLoadedFile() const;
            
            /**
            * @brief Retrieve the next frame to display it.
            * If no video is loaded or there is no new frame, an empty frame will be returned.
            * You should check the returned frame with the method .empty(), which will return true if the frame is empty.
            *
            * @return std::shared_ptr<libtrainsim::Frame> The next frame of the video
            */
            virtual std::shared_ptr<libtrainsim::Frame> getNextFrame();
            
            /**
             * @brief scale a frame to the required buffer size and color format.
             * @note the passed data will no be modified and is read only. The scaled data is in the return value.
             * @param frame The frame data that should be scaled
             * @return std::shared_ptr<libtrainsim::Frame> the scaled frame data
             */
            virtual std::shared_ptr<libtrainsim::Frame> scaleFrame(std::shared_ptr<libtrainsim::Frame> frame);
            
            /**
             * @brief initiate an empty frame.
             * This sets the correct backend and copies the last frame into it.
             * @param frame The frame that should be initiated.
             */
            virtual void initFrame(std::shared_ptr<libtrainsim::Frame> frame);
            
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
