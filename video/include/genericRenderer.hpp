#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include "frame.hpp"

namespace libtrainsim {
    namespace Video{
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
            * @brief jump to the given frame number and refresh the window
            * @param frameNum the frame that will be displayed
            */
            virtual std::shared_ptr<libtrainsim::Frame> gotoFrame(uint64_t frameNum);

            /**
            * @brief get the total number of frames for the loaded video file
            * @return the total number of frames
            *
            */
            virtual uint64_t getFrameCount();
            
            /**
            * @brief Get the Height of the video in pixels
            *
            * @return double
            */
            virtual double getHight();

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
            * @return const libtrainsim::Frame The next frame of the video
            */
            virtual std::shared_ptr<libtrainsim::Frame> getNextFrame();
            
            virtual std::shared_ptr<libtrainsim::Frame> scaleFrame(std::shared_ptr<libtrainsim::Frame> frame);
            
            virtual void initFrame(std::shared_ptr<libtrainsim::Frame> frame);
            
            bool reachedEndOfFile();
        };
    }
}
