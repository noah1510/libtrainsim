#pragma once

#if __has_include("libavcodec/avcodec.h") && __has_include("libavutil/imgutils.h") && __has_include("libavformat/avformat.h") && __has_include("libswscale/swscale.h")
    extern "C"{
        #include <libavcodec/avcodec.h>
        #include <libavutil/imgutils.h>
        #include <libavformat/avformat.h>
        #include <libswscale/swscale.h>
    }
#else 
    #error "Cannot include ffmpeg"
#endif

#include <iostream>
#include "helper.hpp"

namespace libtrainsim {
    namespace Video{
        /**
        * @brief an abstraction on whatever backend is used for handling frames.
        * 
        */
        class LIBTRAINSIM_EXPORT_MACRO Frame{
            private:
                
                AVFrame* frameData = nullptr;

            public:

                /**
                * @brief Construct an empty frame.
                * 
                */
                Frame();
                
                Frame(AVFrame* dat);
                
                /**
                * free all of the data in this Frame object.
                */
                void clear();
                
                /**
                * clear the data of the frame when it is destroyed
                */
                ~Frame();

                /**
                * @brief Checks if the frame data is empty
                * 
                * @return true the frame data is empty
                * @return false the frame data has contents
                */
                bool isEmpty() const;
                
                /**
                * @brief retrieve a pointer to the internal data for the FFmpeg backend
                * 
                */
                AVFrame* data() const;
                
                /**
                * Convert a frame to an AVFrame without needing to call dataFF
                * 
                */
                operator AVFrame*();
                
                /**
                * directly assign an AVFrame to the Frame to set the value
                */
                Frame& operator=(AVFrame* x);
            
        };
    }

}
