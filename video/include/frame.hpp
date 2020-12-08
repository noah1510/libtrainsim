#pragma once

#include "video_config.hpp"

#ifdef HAS_OPENCV_SUPPORT
    #if __has_include("opencv2/opencv.hpp")
        #include "opencv2/opencv.hpp"
    #elif __has_include("opencv4/opencv2/opencv.hpp")
        #include "opencv4/opencv2/opencv.hpp"
    #else
        #undef HAS_OPENCV_SUPPORT
    #endif
#endif

#ifdef HAS_SDL_SUPPORT
    #if  __has_include("SDL2/SDL.h") && __has_include("SDL2/SDL_thread.h")
        #include <SDL2/SDL.h>
        #include <SDL2/SDL_thread.h>
    #else
        #undef HAS_SDL_SUPPORT
    #endif
#endif

#ifdef HAS_FFMPEG_SUPPORT
    #if defined(HAS_SDL_SUPPORT) && __has_include("libavcodec/avcodec.h") && __has_include("libavutil/imgutils.h") && __has_include("libavformat/avformat.h") && __has_include("libswscale/swscale.h")
        extern "C"{
            #include <libavcodec/avcodec.h>
            #include <libavutil/imgutils.h>
            #include <libavformat/avformat.h>
            #include <libswscale/swscale.h>
        }
    #else 
        #undef HAS_FFMPEG_SUPPORT
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
        
        #ifdef HAS_FFMPEG_SUPPORT
        ///use ffmpeg with any found windowing system
        ffmpeg = 2,
        #endif
        
        #if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_SDL_SUPPORT)
        ///use ffmpeg with an sdl window as backend
        ffmpeg_sdl = 3,
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
            
            /**
             * delete the contents of the opencv frame container
             */
            void clearCV();
            #endif
            
            #ifdef HAS_FFMPEG_SUPPORT
            
            /**
             *  @brief the actual data of the frame for the ffmpeg backend
             */
            AVFrame* frameDataFF = nullptr;
            
            /**
             * delete the contents of the ffmpeg frame container
             */
            void clearFF();
            
            /**
             * create an empty ffmpeg frame
             */
            void createEmptyFF();
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
            
            #ifdef HAS_FFMPEG_SUPPORT
            /**
             * @brief Construct a new frame from given data
             * 
             * @param dat the data this frame will contain
             */
            Frame(AVFrame* dat);
            
            /**
             * @brief retrieve a pointer to the internal data for the FFmpeg backend
             * 
             */
            AVFrame* dataFF();
            
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
            
            /**
             * clear the data of the frame when it is destroyed
             */
            ~Frame();
            
            /**
             * @brief set the video backend of an empty frame to the given backend
             * @param newBackend the new backend this frame should have
             */
            void setBackend(VideoBackends newBackend);
    };

}
