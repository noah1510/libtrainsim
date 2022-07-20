#include "control.hpp"

libtrainsim::control::input_handler::input_handler(const std::filesystem::path& URI) noexcept(false): serial{URI}{
    keys = libtrainsim::control::keymap();
}

libtrainsim::control::keymap& libtrainsim::control::input_handler::Keymap() noexcept {
    return keys;
}

std::string libtrainsim::control::input_handler::getKeyFunction() noexcept {
    char pressedKey = '\0';
    
    #ifdef HAS_VIDEO_SUPPORT


        SDL_Event event;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        SDL_PollEvent(&event);

        if(event.type == SDL_QUIT){
            return "CLOSE";
        };

        if(event.type == SDL_KEYDOWN){
            pressedKey = event.key.keysym.sym;
        };

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

void libtrainsim::control::input_handler::update() noexcept{

    auto function = getKeyFunction();
    
    if(function == "CLOSE"){
        shouldClose = true;
    }

    //if there is harware input update most flags from there
    if(serial.IsConnected()){
        serial.update();

        shouldEmergencyBreak = serial.get_emergencyflag();
        currentInputAxis = serial.get_slvl();
    }else{

        if(function == "EMERGENCY_BREAK"){
            shouldEmergencyBreak = true;
        }

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
    }  

}


