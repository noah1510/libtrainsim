#pragma once

#include "frame.hpp"

#include "video_config.hpp"

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
    
    namespace libtrainsim {
        
        class ffmpegFrame : public Frame{
            private:
                /**
                *  @brief the actual data of the frame for the ffmpeg backend
                */
                AVFrame* frameDataFF = nullptr;
                
            public:
                
                /**
                * create an empty ffmpeg frame
                */
                ffmpegFrame();
                
                /**
                * @brief Construct a new frame from given data
                * 
                * @param dat the data this frame will contain
                */
                ffmpegFrame(AVFrame* dat);
                
                /**
                * @brief retrieve a pointer to the internal data for the FFmpeg backend
                * 
                */
                AVFrame* dataFF() const;
                
                /**
                * delete the contents of the ffmpeg frame container
                */
                void clear();
                
                /**
                * Convert a frame to an AVFrame without needing to call dataFF
                * 
                */
                operator AVFrame*();
                
                /**
                * directly assign an AVFrame to the Frame to set the value
                */
                ffmpegFrame& operator=(AVFrame* x);
                
                
                /**
                * @brief Checks if the frame data is empty
                * 
                * @return true the frame data is empty
                * @return false the frame data has contents
                */
                bool isEmpty() const;
            
        };
        
    }
    
#endif
