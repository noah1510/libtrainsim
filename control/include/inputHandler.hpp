#pragma once

// This has to be included after video.hpp to work properly on windows
// Seems to be a problem with the was rs232 includes windows.h which causes
// a conflict with GTK
#include "serialcontrol.hpp"

namespace libtrainsim {

    /**
     * @brief This namespace has all of the control related functions.
     * At the moment these are the input_handler and the keymap.
     *
     */
    namespace control {
        /**
         * @brief this class provides an interface to control the train easily independent of the used windowsing system.
         * The following Key functions are defined:
         *
         *   * CLOSE -> indicates that the windows should close
         *   * TERMINATE -> indicates that all windows should be closed and the program ends
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
        class LIBTRAINSIM_EXPORT_MACRO input_handler : public SimpleGFX::eventHandle {
          private:
            /**
             * @brief a mutex to control the data access across threads
             */
            std::shared_mutex dataMutex;

            std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf;

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
             * @brief a bool to set if the simulator is running
             */
            bool running = false;

            /**
             * @brief the serial interface to the connected hardware input
             *
             */
            std::unique_ptr<serialcontrol> serial;

#ifdef HAS_VIDEO_SUPPORT
            std::shared_ptr<SimpleGFX::SimpleGL::eventPollerGtkKeyboard> keyboardPoller = nullptr;
#endif

          public:
            /**
             * @brief Construct a new input handler object
             * By default w accelerates, s breaks, p sets the emergency break and ESC closes the program.
             * If hardware input is available it will be used by default instead of keyboard accelleration and breaking.
             *
             * @param URI The location of the serial configuration file (should be given by the settings)
             */
            input_handler(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf) noexcept(false);

            /**
             * @brief destory the input handler
             */
            ~input_handler();

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
            core::input_axis getSpeedAxis() noexcept;

            /**
             * @brief closes the serial connection and resets the axis
             */
            void resetFlags();

            /**
             * @brief starts the serial connection and starts reading the keyboard
             */
            void startSimulation();

            std::shared_ptr<SimpleGFX::trackedFuture<uint64_t>> registerWithEventManager(SimpleGFX::eventManager* _manager, int priority) override;
            void     unregister() override;
            bool     onEvent(const SimpleGFX::inputEvent& event) override;


#ifdef HAS_VIDEO_SUPPORT
            std::shared_ptr<SimpleGFX::SimpleGL::eventPollerGtkKeyboard> getKeyboardPoller();
#endif
        };
    } // namespace control
} // namespace libtrainsim
