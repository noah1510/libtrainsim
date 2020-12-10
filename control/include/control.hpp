#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include "types.hpp"
#include "libtrainsim_config.hpp"

#ifdef HAS_VIDEO_SUPPORT
    #if __has_include("video.hpp")
        #include "video.hpp"
    #else
        #undef HAS_VIDEO_SUPPORT
    #endif
#endif

namespace libtrainsim {
    /**
     * @brief this class provides an interface to control the train easily independent of the used windowsing system.
     * @warning this interface is only really useful is you use libtrainsim::video to handle window management otherwise it is not possible to retrieve the currently pressed keys.
     * @todo implment input from (analog) hardware controls.
     */
    class control{
        private:
            /**
             * @brief Construct a new control object
             * 
             */
            control(void);

            /**
             * @brief Get the Instance of this singleton
             * 
             * @return control& a reference to the single instance
             */
            static control& getInstance(){
                static control instance;
                return instance;
            };

            ///the implementation of hello()
            std::string hello_impl() const;

        public:
            /**
             * @brief return a string to test if the singleton works correctly
             * 
             * @return std::string the starting message
             */
            static std::string hello(){
                return getInstance().hello_impl();
            }

            /**
             * @brief check if a specific key is pressed
             * 
             * @param Key the key to check
             * @return true the key is pressed
             * @return false the key is not pressed
             */
            static bool isKeyPressed(char Key){
                return getKey() == Key;
            }

            /**
             * @brief Get the Key that is currently pressed
             * 
             * @return char the key that is pressed
             */
            static char getKey(){
                #ifdef HAS_VIDEO_SUPPORT
                
                switch(libtrainsim::video::getBackend()){
                    #ifdef HAS_OPENCV_SUPPORT
                    case(opencv):
                        return cv::waitKey(1);
                    #endif
                    #ifdef HAS_SDL_SUPPORT
                    case(ffmpeg):
                    case(ffmpeg_sdl):
                        SDL_Event event;
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        SDL_PollEvent(&event);
                        if(event.type == SDL_QUIT){return core::KEY_ESCAPE;};
                        if(event.type == SDL_KEYDOWN){return event.key.keysym.sym;};
                        break;
                    #endif
                    case(none):
                    default:
                        break;
                }
                #endif
                
                return '\0';
            }

            /**
             * @brief Get the action that is currently performed.
             * 
             * @return core::actions the current action
             */
            static core::actions getCurrentAction(){
                switch(getKey()){
                    case(core::KEY_ESCAPE):
                        return core::ACTION_CLOSE;
                    case(core::KEY_ACCELERATE):
                        return core::ACTION_ACCELERATE;
                    case(core::KEY_BREAK):
                        return core::ACTION_BREAK;
                    case('\0'):
                        return core::ACTION_NONE;
                    default:
                        return core::ACTION_OTHER;       
                }

                return core::ACTION_NONE;
            }

            /**
             * @brief This function returns the acelleration on a scale from -1.0 to 1.0.
             * @note at the moment there is no support for any analog input so it is either 1.0 or -1.0.
             * @return double the amount of acceleration between -1.0 and 1.0 
             */
            static double getScaledAcceleration(){
                //if there is harware input return the scaled acceleration
                #ifdef HAS_HW_INPUT_SUPPORT

                #endif

                auto action = getCurrentAction();
                if (action == core::ACTION_ACCELERATE){return 1.0;};
                if (action == core::ACTION_BREAK){return -1.0;};

                return 0.0;
            }
    };
}

