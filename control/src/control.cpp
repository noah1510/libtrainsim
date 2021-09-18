#include "control.hpp"

libtrainsim::control::input_handler::input_handler(void){
    keys = libtrainsim::control::keymap();
}

std::string libtrainsim::control::input_handler::hello() const{
    return "Hello from the control singleton";
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
            }
        #endif

    #endif
    
    auto keyFunction = keys.getFunction(pressedKey);
    
    return keyFunction;
}

libtrainsim::core::actions libtrainsim::control::input_handler::getCurrentAction(){
    using namespace libtrainsim::core;
    auto keyFunction = getKeyFunction();
    
    if(keyFunction == "NONE"){
        return ACTION_NONE;
    }
    
    if(keyFunction == "CLOSE"){
        return ACTION_CLOSE;
    }
    
    if(keyFunction == "ACCELERATE"){
        return ACTION_ACCELERATE;
    }
    
    if(keyFunction == "BREAK"){
        return ACTION_BREAK;
    }
    
    if(keyFunction == "OTHER"){
        return ACTION_OTHER;
    }

    return ACTION_NONE;
}

libtrainsim::core::input_axis libtrainsim::control::input_handler::getSpeedAxis(){
    //if there is harware input return the scaled acceleration
    #ifdef HAS_HW_INPUT_SUPPORT

    #endif

    auto function = getKeyFunction();
    if (function == "ACCELERATE"){return core::input_axis{1.0};};
    if (function == "BREAK"){return core::input_axis{-1.0};};

    return core::input_axis{0.0};
}
