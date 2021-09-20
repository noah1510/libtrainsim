#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <memory>

#include <map>
#include <string>
#include <vector>

#include "libtrainsim_config.hpp"
#ifdef HAS_VIDEO_SUPPORT
    #if __has_include("video.hpp")
        #include "video.hpp"
    #else
        #undef HAS_VIDEO_SUPPORT
    #endif
#endif

#ifdef HAS_GLFW_SUPPORT
namespace libtrainsim {
    namespace control{
        class glfwKeyTranslation{
        private:
            glfwKeyTranslation();
            
            static glfwKeyTranslation& getInstance(){
                static glfwKeyTranslation _Instance{};
                return _Instance;
            }
            
            /**
            * @brief The underlying map that assigns a char to each glfwKey.
            * 
            */
            std::map<int, char> _keymap;
            
            char getKeyAsChar_impl(int glfwKey);
            
            int getCharAsKey_impl(char charKey);
        public:
            static char getKeyAsChar(int glfwKey){
                return getInstance().getCharAsKey_impl(glfwKey);
            }
            
            static int getCharAsKey(char charKey){
                return getInstance().getCharAsKey_impl(charKey);
            }
        };
    }
}
#endif
