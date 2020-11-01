#pragma once

#include "frame.hpp"
#include <filesystem>
#include <string>

namespace libtrainsim::backend{
    #ifdef HAS_OPENCV_SUPPORT
    class videoOpenCV{
        private:
            /**
            * @brief This variable saves the state of the video capture device aka the video that is rendered.
            * 
            */
            std::unique_ptr<cv::VideoCapture> videoCap;

            ///the backend for the video capture class.
            cv::VideoCaptureAPIs backend = cv::CAP_ANY;

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
            ~videoOpenCV();

            /**
             * @brief Retrieve the next frame to display it. 
             * If no video is loaded or there is no new frame, an empty frame will be returned.
             * You should check the returned frame with the method .empty(), which will return true if the frame is empty.
             * 
             * @return const libtrainsim::Frame The next frame of the video
             */
            const libtrainsim::Frame getNextFrame();

            /**
             * @brief Get a property of the internal videoCapture object.
             * @note Reading / writing properties involves many layers. Some unexpected result might happens along this chain. See cv::VideoCapture::get() for more information.
             * @param prop the wanted property
             * @return double the value of that property or 0 if it is nor supported
             */
            double getVideoProperty(const cv::VideoCaptureProperties& prop) const;

            /**
             * @brief Set a property of the internal videoCapture object.
             * @note Even if it returns true this doesn't ensure that the property value has been accepted by the capture device. See note in cv::VideoCapture::get()
             * @param prop the wanted property
             * @param value the value the property should have 
             * @return true if the property is supported by backend used by the VideoCapture instance and the videoCapture is opened.
             */
            bool setVideoProperty(const cv::VideoCaptureProperties& prop, double value);

            /**
             * @brief Set the Backend of the opencv video capture
             * @warning this is useless if called after load 
             * 
             * @param newBackend 
             */
            void setBackend(cv::VideoCaptureAPIs newBackend);

            /**
             * @brief Get the Backend of the video capture
             * 
             * @return cv::VideoCaptureAPIs the video capture backend
             */
            cv::VideoCaptureAPIs getBackend();

            /**
             * @brief Load a video file into the video management.
             * 
             * @param uri The uri of the file.
             * @return true file sucessfully loaded
             * @return false error while loading file
             */
            bool load(const std::filesystem::path& uri);

            /**
             * @brief Create a Window with a given name
             * 
             * @param windowName the name of the window
             */
            void createWindow(const std::string& windowName);

            /**
             * @brief refresh the window but do not display something new
             * 
             */
            void refreshWindow();

            /**
             * @brief display a new frame on the window
             * 
             * @param newFrame the new frame to be displayed
             */
            void displayFrame(const Frame& newFrame);

            /**
             * @brief update the window contents to the latest contentst of the video
             * 
             */
            void updateWindow();

    };

    #endif

}