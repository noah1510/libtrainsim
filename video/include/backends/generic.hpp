#pragma once

#include "frame.hpp"
#include <filesystem>
#include <string>

namespace libtrainsim::backend{
    /**
     * @brief This is a generic class every specific video backend can extent from.
     * It specifies the way every backend should interact with the video sigleton.
     * 
     */
    class videoGeneric{
        protected:

            /**
             * @brief This is that path to the currently loaded file.
             * 
             */
            std::filesystem::path loadedFile;

            /**
             * @brief the name of the current window
             * 
             */
            std::string currentWindowName = "";

            /**
             * @brief the frame that was displayed last.
             * 
             */
            Frame lastFrame;
            
            /**
             * @brief The number of the frame that is currently displayed
             */
            uint64_t currentFrameNumber = 0;
            
            /**
             * @brief display a new frame on the window
             * 
             * @param newFrame the new frame to be displayed
             */
            virtual void displayFrame(const Frame& newFrame);
            
            
            /**
             * @brief Retrieve the next frame to display it. 
             * If no video is loaded or there is no new frame, an empty frame will be returned.
             * You should check the returned frame with the method .empty(), which will return true if the frame is empty.
             * 
             * @return const libtrainsim::Frame The next frame of the video
             */
            virtual const libtrainsim::Frame getNextFrame();

        public:

            /**
             * @brief Destroy the video OpenCV object
             * 
             */
            virtual ~videoGeneric();

            /**
             * @brief Load a video file into the video management.
             * 
             * @param uri The uri of the file.
             * @return true file sucessfully loaded
             * @return false error while loading file
             */
            virtual bool load(const std::filesystem::path& uri);

            /**
             * @brief Create a Window with a given name
             * 
             * @param windowName the name of the window
             */
            virtual void createWindow(const std::string& windowName);

            /**
             * @brief refresh the window but do not display something new
             * 
             */
            virtual void refreshWindow();
            
            /**
             * @brief jump to the given frame number and refresh the window
             * @param frameNum the frame that will be displayed
             */
            virtual void gotoFrame(uint64_t frameNum);
            
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

    };

}
