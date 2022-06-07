#pragma once

#include "video_config.hpp"
#include <iostream>

#ifdef HAS_FFMPEG_SUPPORT
    #if __has_include("libavcodec/avcodec.h") && __has_include("libavutil/imgutils.h") && __has_include("libavformat/avformat.h") && __has_include("libswscale/swscale.h")
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
    
    namespace Video{
        /**
         * @brief a list of the available renderer Backends
         */
        enum RendererBackends{
            
            /// No selected backend
            renderer_none = 0,
            
            #ifdef HAS_FFMPEG_SUPPORT
            ///use ffmpeg with any found windowing system
            renderer_ffmpeg = 2,
            #endif
        };
    
    }

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
            Video::RendererBackends currentBackend = Video::renderer_none;
            
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
            AVFrame* dataFF() const;
            
            /**
             * Convert a frame to an AVFrame without needing to call dataFF
             * 
             */
            operator AVFrame*();
            
            /**
             * directly assign an AVFrame to the Frame to set the value
             */
            Frame& operator=(AVFrame* x);
            
            #endif

            /**
             * @brief Get the Backend used for this frame
             * 
             * @return VideoBackends the backend of the frame
             */
            Video::RendererBackends getBackend() const;

            /**
             * @brief Construct an empty frame.
             * 
             */
            Frame();
            
            /**
             * free all of the data in this Frame object.
             */
            void clear();
            
            /**
             * clear the data of the frame when it is destroyed
             */
            ~Frame();
            
            /**
             * @brief set the video backend of an empty frame to the given backend
             * @param newBackend the new backend this frame should have
             */
            void setBackend(Video::RendererBackends newBackend);

            /**
             * @brief Checks if the frame data is empty
             * 
             * @return true the frame data is empty
             * @return false the frame data has contents
             */
            bool isEmpty() const;
    };

}
