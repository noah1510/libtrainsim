#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "input_axis.hpp"
#include "keymap.hpp"
#include "libtrainsim_config.hpp"
#include "serialcontrol.hpp"
#include "types.hpp"

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
        *   * CLOSE -> indicates that the windows should close
        *   * TERMINATE -> indicates that all windows shoudl be closed and the program ends
        *   * OTHER -> some other random event
        *   * BREAK -> indicates the train should break
        *   * ACCELERATE -> indicates the train should accelerate
        *   * EMERGENCY_BREAK -> indicated the emergency break should be activated
        *
        * Using the Keymap() function you can get a reference to the keymap of the input_handler and register addtional keys and functions,
        * that can be handled by an implementation of the simulator.
        * 
        * @warning this interface is only really useful is you use libtrainsim::video to handle window management otherwise
        * it is not possible to retrieve the currently pressed keys. If libtrainsim::video is not available the serialcontrol
        * part still works, its just that this can no longer detect window events and the closing flag is always false.
        */
        class LIBTRAINSIM_EXPORT_MACRO input_handler{
            private:
                /**
                 * @brief the keymap used for the input_handler module
                 * 
                 */
                control::keymap keys;
            
                /**
                 * @brief the current speed level for keyboard controls
                 */
                libtrainsim::core::input_axis currentInputAxis = 0.0;
                
                /**
                 * @brief a bool to indicate if the window should be closed
                 */
                bool shouldClose = false;
                
                /**
                 * @brief a bool to indicate if all windows need to be closed
                 */
                bool shouldTeminate = false;
                
                /**
                 * @brief a bool to indicate if the emergency break should activate
                 */
                bool shouldEmergencyBreak = false;

                /**
                 * @brief the serial interface to the connected hardware input
                 * 
                 */
                std::unique_ptr<serialcontrol> serial;

            public:

               /**
                * @brief Construct a new input handler object
                * By default w accelerates, s breaks, p sets the emergency break and ESC closes the program.
                * If hardware input is available it will be used by default instead of keyboard accelleration and breaking.
                * 
                * @param URI The location of the serial configuration file (should be given by the settings)
                */
                input_handler(const std::filesystem::path& URI) noexcept(false);
                
                /**
                 * @brief destory the input handler
                 */
                ~input_handler();

                /**
                 * @brief Get the function of the currently pressed keys.
                 * 
                 * @return std::string the function that should be performed.
                 */
                std::vector<std::string> getKeyFunctions() noexcept;
                
                /**
                 * @brief access the internal keymap to change the input configuration.
                 * @note the keymap is only useful for keyboard input
                 * 
                 * @return keymap& a reference to the internal keymap
                 */
                keymap& Keymap() noexcept;
                
                /**
                 * @brief return true if getSpeedAxis came across a close command
                 */
                bool closingFlag() noexcept;
                
                /**
                 * @brief return true if getSpeedAxis came across a emergency break command
                 */
                bool emergencyFlag() noexcept;
                
                /**
                 * @brief Get the Speed Axis of the current input.
                 * If a keyboard is used, the value will be -1.0, 0.0 or 1.0.
                 * Hardware controls can provide any value between -1.0 and 1.0.
                 * This input axis can be passed directly to the physics component.
                 * 
                 * @return core::input_axis The input axis which desribes how much the train should accelerate/break.
                 */
                core::input_axis getSpeedAxis() const noexcept;

                /**
                 * @brief update all of the flags and the speed axis value
                 * @warning This needs to be called on the render thread. Otherwise this function throws an error.
                 */
                void update();
        };
    }
}

