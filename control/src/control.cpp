#include "control.hpp"

libtrainsim::control::input_handler::input_handler() noexcept(false){
    keys = libtrainsim::control::keymap();
}

libtrainsim::control::keymap& libtrainsim::control::input_handler::Keymap(){
    return keys;
}

std::string libtrainsim::control::input_handler::getKeyFunction() {
    char pressedKey = '\0';
    
    #ifdef HAS_VIDEO_SUPPORT
    
        auto backend = libtrainsim::video::getBackend().windowType;

        #ifdef HAS_OPENCV_SUPPORT
            if ( backend == libtrainsim::Video::window_opencv){

                pressedKey = cv::waitKey(1);
            }
        #endif

        #ifdef HAS_SDL_SUPPORT
            if ( backend == libtrainsim::Video::window_sdl){

                SDL_Event event;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                SDL_PollEvent(&event);

                if(event.type == SDL_QUIT){
                    return "CLOSE";
                };

                if(event.type == SDL_KEYDOWN){
                    pressedKey = event.key.keysym.sym;
                };
            }
        #endif
        
        #ifdef HAS_GLFW_SUPPORT
            if ( backend == libtrainsim::Video::window_glfw){
                auto _window = video::getGLFWwindow();
                
                if (glfwWindowShouldClose(_window)){
                    return "CLOSE";
                }
                
                glfwWaitEventsTimeout(0.001);
                
                auto keyList = keys.getAllKeys();
                for (auto key: keyList){
                    auto glfwKey = glfwKeyTranslation::getCharAsKey(key);
                    if (glfwGetKey(_window, glfwKey) == GLFW_PRESS){
                        return keys.getFunction(key);
                    }
                }
                
            }
        #endif

    #endif
    
    auto keyFunction = keys.getFunction(pressedKey);
    
    return keyFunction;
}

libtrainsim::core::input_axis libtrainsim::control::input_handler::getSpeedAxis() const noexcept {
    return currentInputAxis;
}

bool libtrainsim::control::input_handler::closingFlag() const noexcept {
    return shouldClose;
}

bool libtrainsim::control::input_handler::emergencyFlag() const noexcept {
    return shouldEmergencyBreak;
}

void libtrainsim::control::input_handler::update(){
    //if there is harware input update most flags from there


    auto function = getKeyFunction();
    
    if (function == "ACCELERATE"){
        currentInputAxis += 0.1;
        if(abs(currentInputAxis.get()) < 0.07){
            currentInputAxis = 0.0;
        }
        
    } else if (function == "BREAK"){
        currentInputAxis -= 0.1;
        if(abs(currentInputAxis.get()) < 0.07){
            currentInputAxis = 0.0;
        }
    }
    
    if(function == "CLOSE"){
        shouldClose = true;
    }
    
    if(function == "EMERGENCY_BREAK"){
        shouldEmergencyBreak = true;
    }
}


