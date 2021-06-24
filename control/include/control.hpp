#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <memory>

#include "types.hpp"
#include "libtrainsim_config.hpp"
#include "input_axis.hpp"
#include "keymap.hpp"

#ifdef HAS_VIDEO_SUPPORT
    #if __has_include("video.hpp")
        #include "video.hpp"
    #else
        #undef HAS_VIDEO_SUPPORT
    #endif
#endif

namespace libtrainsim {
    
    namespace control{
        /**
        * @brief this class provides an interface to control the train easily independent of the used windowsing system.
        * @warning this interface is only really useful is you use libtrainsim::video to handle window management otherwise it is not possible to retrieve the currently pressed keys.
        * @todo implment input from (analog) hardware controls.
        */
        class input_handler{
            private:
                //the keymap used for this control module
                control::keymap keys;

            public:
                /**
                * @brief Construct a new control object
                * 
                */
                input_handler();
                
                /**
                * @brief return a string to test if the singleton works correctly
                * 
                * @return std::string the starting message
                */
                std::string hello() const;

                std::string getKeyFunction();
                
                keymap& Keymap();
                
                /**
                * @brief Get the action that is currently performed.
                * 
                * @return core::actions the current action
                */
                core::actions getCurrentAction();
                
                libtrainsim::core::input_axis getSpeedAxis();
        };
    }
}

