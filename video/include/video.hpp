#pragma once

#include <memory>
#include <string>
#include <filesystem>

#if __has_include("opencv2/opencv.hpp")
    #include "opencv2/opencv.hpp"
#elif __has_include("opencv4/opencv2/opencv.hpp")
    #include "opencv4/opencv2/opencv.hpp"
#endif

namespace libtrainsim {
    /**
     * @brief This class is resposiblie for managing all the video material.
     * 
     */
    class video{
        private:
            /**
             * @brief Construct a new video object (must only be called by getInstance when necessary)
             * 
             */
            video(void);

            /**
             * @brief Destroy the video object, on destruction everything will be reset.
             * 
             */
            ~video(){
                reset();
            }

            /**
             * @brief reset the singleton to the initial state
             * 
             */
            void reset();

            /**
             * @brief Get the Instance of the singleton
             * 
             * @return video& a reference to the object
             */
            static video& getInstance(){
                static video instance;
                return instance;
            };

            /**
             * @brief The implementation of hello()
             * 
             */
            std::string hello_impl() const;

            /**
             * @brief The implementation of load look @load for details
             */
            bool load_impl(const std::filesystem::path& uri);

            /**
             * @brief This is that path to the currently loaded file.
             * 
             */
            std::filesystem::path loadedFile;

            /**
             * @brief This variable saves the state of the video capture device aka the video that is rendered.
             * 
             */
            std::unique_ptr<cv::VideoCapture> videoCap;

            ///The implementation for the getNextFrame method
            const cv::Mat getNextFrame_impl();

            ///The implementation for the getFilePath method
            std::filesystem::path getFilePath_impl() const;

            ///The implementation of getVideoProperty
            double getVideoProperty_impl(const cv::VideoCaptureProperties& prop) const;

            ///the implementation of setVideoProperty
            bool setVideoProperty_impl(const cv::VideoCaptureProperties& prop, double value);

        public:
            /**
             * @brief This function is used to test if libtrainsim::video was correctly linked and just prints hello world
             * 
             */
            static std::string hello(){
                return getInstance().hello_impl();
            }

            /**
             * @brief Load a video file into the video management.
             * 
             * @param uri The uri of the file.
             * @return true file sucessfully loaded
             * @return false error while loading file
             */
            static bool load(const std::filesystem::path& uri){
                return getInstance().load_impl(uri);
            }

            /**
             * @brief Get the File Path of the loaded video file
             * 
             * @return std::filesystem::path the filepath to the current video file
             */
            static std::filesystem::path getFilePath(){
                return getInstance().getFilePath_impl();
            }

            /**
             * @brief Get a property of the internal videoCapture object.
             * @note Reading / writing properties involves many layers. Some unexpected result might happens along this chain. See cv::VideoCapture::get() for more information.
             * @param prop the wanted property
             * @return double the value of that property or 0 if it is nor supported
             */
            static double getVideoProperty(const cv::VideoCaptureProperties& prop){
                return getInstance().getVideoProperty_impl(prop);
            }

            /**
             * @brief Set a property of the internal videoCapture object.
             * @note Even if it returns true this doesn't ensure that the property value has been accepted by the capture device. See note in cv::VideoCapture::get()
             * @param prop the wanted property
             * @param value the value the property should have 
             * @return true if the property is supported by backend used by the VideoCapture instance and the videoCapture is opened.
             */
            static bool setVideoProperty(const cv::VideoCaptureProperties& prop, double value){
                return getInstance().setVideoProperty_impl(prop, value);
            }

            /**
             * @brief Get the Width of the video in pixels
             * 
             * @return double 
             */
            static double getWidth(){
                return getVideoProperty(cv::CAP_PROP_FRAME_WIDTH);
            }

            /**
             * @brief Get the Height of the video in pixels
             * 
             * @return double 
             */
            static double getHight(){
                return getVideoProperty(cv::CAP_PROP_FRAME_HEIGHT);
            }

            /**
             * @brief Retrieve the next frame to display it. 
             * If no video is loaded or there is no new frame, an empty frame will be returned.
             * You should check the returned frame with the method .empty(), which will return true if the frame is empty.
             * 
             * @return const cv::Mat The next frame of the video
             */
            static const cv::Mat getNextFrame(){
                return getInstance().getNextFrame_impl();
            }

    };
}

