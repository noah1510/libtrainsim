#pragma once

#include "frame.hpp"
#include "genericRenderer.hpp"

#include <filesystem>
#include <string>

namespace libtrainsim {
    namespace Video{
        #if defined(HAS_OPENCV_SUPPORT)
        
            class openCVRenderer : public genericRenderer{
            private:
                /**
                * @brief This variable saves the state of the video capture device aka the video that is rendered.
                * 
                */
                std::unique_ptr<cv::VideoCapture> videoCap;

                ///the backend for the video capture class.
                cv::VideoCaptureAPIs backend = cv::CAP_ANY;
                
            protected:
                
            public:
                
                /**
                * @brief Destroy the video FF_SDL object
                * 
                */
                ~openCVRenderer();

                //The functions below are just reimplemented from video_generic

                bool load(const std::filesystem::path& uri);
                const libtrainsim::Frame gotoFrame(uint64_t frameNum);
                uint64_t getFrameCount();
                double getHight();
                double getWidth();
                
                
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
                * @brief Retrieve the next frame to display it.
                * If no video is loaded or there is no new frame, an empty frame will be returned.
                * You should check the returned frame with the method .empty(), which will return true if the frame is empty.
                *
                * @return const libtrainsim::Frame The next frame of the video
                */
                const libtrainsim::Frame getNextFrame();
            };
        
        #endif
    }
}

