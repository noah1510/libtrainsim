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

        public:

            /**
             * @brief Destroy the video OpenCV object
             * 
             */
            virtual ~videoGeneric();

            /**
             * @brief Retrieve the next frame to display it. 
             * If no video is loaded or there is no new frame, an empty frame will be returned.
             * You should check the returned frame with the method .empty(), which will return true if the frame is empty.
             * 
             * @return const libtrainsim::Frame The next frame of the video
             */
            virtual const libtrainsim::Frame getNextFrame();

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
             * @brief display a new frame on the window
             * 
             * @param newFrame the new frame to be displayed
             */
            virtual void displayFrame(const Frame& newFrame);

            /**
             * @brief update the window contents to the latest contentst of the video
             * 
             */
            virtual void updateWindow();

    };

}
