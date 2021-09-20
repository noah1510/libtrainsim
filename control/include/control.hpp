#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <memory>

#include "types.hpp"
#include "libtrainsim_config.hpp"
#include "input_axis.hpp"
#include "keymap.hpp"
#include "glfwKeyTranslation.hpp"

#ifdef HAS_VIDEO_SUPPORT
    #if __has_include("video.hpp")
        #include "video.hpp"
    #else
        #undef HAS_VIDEO_SUPPORT
    #endif
#endif

namespace libtrainsim {
    
    /**
     * @brief This namespace has all of the control related functions.
     * At the moment these are the input_handler and the keymap.
     * 
     */
    namespace control{
        /**
        * @brief this class provides an interface to control the train easily independent of the used windowsing system.
        * The following Key functions are defined:
        *
        *   * NONE -> Does nothing
        *   * CLOSE -> indicates that the windows chould close and the program should end
        *   * OTHER -> some other random event
        *   * BREAK -> indicates the train should break
        *   * ACCELERATE -> indicates the train should accelerate
        *
        * Using the Keymap() function you can get a reference to the keymap of the input_handler and register addtional keys and functions,
        * that can be handled by an implementation of the simulator.
        *
        * @warning this interface is only really useful is you use libtrainsim::video to handle window management otherwise
        * it is not possible to retrieve the currently pressed keys. You can use this a a base to implement you own input_handler
        * if you some other window management.
        * @todo implment input from (analog) hardware controls.
        */
        class input_handler{
            private:
                /**
                 * @brief the keymap used for the input_handler module
                 * 
                 */
                control::keymap keys;

            public:
                /**
                * @brief Construct a new input_handler object.
                * By default w accelerates, s breaks and ESC closes the program.
                * 
                */
                input_handler();
                
                /**
                * @brief return a string to test if the singleton works correctly
                * 
                * @return std::string the starting message
                */
                std::string hello() const;

                /**
                 * @brief Get the function of the currently pressed key.
                 * 
                 * @return std::string the function that should be performed.
                 */
                std::string getKeyFunction();
                
                /**
                 * @brief access the internal keymap to change the input configuration.
                 * @note the keymap is only useful for keyboard input
                 * 
                 * @return keymap& a reference to the internal keymap
                 */
                keymap& Keymap();
                
                /**
                * @brief Get the action that is currently performed.
                * @deprecated use the getKeyFunction() function since this will be removed in a future version.
                * 
                * @return core::actions the current action
                */
                core::actions getCurrentAction();
                
                /**
                 * @brief Get the Speed Axis of the current input.
                 * If a keyboard is used, the value will be -1.0, 0.0 or 1.0.
                 * Hardware controls can provide any value between -1.0 and 1.0.
                 * This input axis can be passed directly to the physics component.
                 * 
                 * @todo implement the step by step increments that the simulator does in this function for keyboard inputs.
                 * @return core::input_axis The input axis which desribes how much the train should accelerate/break.
                 */
                core::input_axis getSpeedAxis();
        };
    }
}

