#pragma once

#include "video_config.hpp"

#ifdef HAS_OPENCV_SUPPORT
    #if __has_include("opencv2/opencv.hpp")
        #include "opencv2/opencv.hpp"
    #elif __has_include("opencv4/opencv2/opencv.hpp")
        #include "opencv4/opencv2/opencv.hpp"
        #define HAS_OPENCV_SUPPORT
    #endif
#endif

namespace libtrainsim {

    /**
     * @brief a list of the available video backends.
     * 
     */
    enum VideoBackends{
        /// No selected backend
        none = 0,

        #ifdef HAS_OPENCV_SUPPORT
        /// use opencv as video backend
        opencv = 1,
        #endif
    };

    /**
     * @brief an abstraction on whatever backend is used for handling frames.
     * 
     */
    class Frame{
        private:
            /**
             * @brief the video backend used for this frame.
             * 
             */
            VideoBackends currentBackend = none;

            #ifdef HAS_OPENCV_SUPPORT

            /**
             * @brief the actual data of the frame for the opencv backend
             * 
             */
            cv::UMat frameDataCV;
            #endif
        public:
            #ifdef HAS_OPENCV_SUPPORT
            /**
             * @brief Construct a new Frame from given data.
             * 
             * @param frameDat the data this frame contains.
             */
            Frame(const cv::UMat& frameDat);

            /**
             * @brief retrive the internal frame data for the opencv backend.
             * 
             * @return cv::UMat* the internal frame data 
             */
            cv::UMat dataCV() const;
            #endif

            /**
             * @brief Get the Backend used for this frame
             * 
             * @return VideoBackends the backend of the frame
             */
            VideoBackends getBackend() const;

            /**
             * @brief Construct an empty frame.
             * 
             */
            Frame();
    };

}
