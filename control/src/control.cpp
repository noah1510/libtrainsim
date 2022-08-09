#include "control.hpp"

libtrainsim::control::input_handler::input_handler(const std::filesystem::path& URI) noexcept(false): serial{URI}{
    keys = libtrainsim::control::keymap();
}

libtrainsim::control::keymap& libtrainsim::control::input_handler::Keymap() noexcept {
    return keys;
}

std::vector<std::string> libtrainsim::control::input_handler::getKeyFunctions() noexcept {
    std::vector<std::string> functions;
    
    #ifdef HAS_VIDEO_SUPPORT

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            
            ImGui_ImplSDL2_ProcessEvent(&event);

            if(event.type == SDL_QUIT){
                functions.emplace_back("CLOSE");
            };

            if(event.type == SDL_KEYDOWN){
                functions.emplace_back(keys.getFunction(event.key.keysym.sym));
            };
        }
        
    #endif
    
    return functions;
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

    auto function = getKeyFunctions();
    
    if(libtrainsim::core::Helper::contains<std::string>(function,"CLOSE")){
        shouldClose = true;
    }

    //if there is harware input update most flags from there
    if(serial.IsConnected()){
        serial.update();

        shouldEmergencyBreak = serial.get_emergencyflag();
        currentInputAxis = serial.get_slvl();
    }else{

        if(libtrainsim::core::Helper::contains<std::string>(function,"EMERGENCY_BREAK")){
            shouldEmergencyBreak = true;
        }

        if (libtrainsim::core::Helper::contains<std::string>(function,"ACCELERATE")){
            currentInputAxis += 0.1;
            if(abs(currentInputAxis.get()) < 0.07){
                currentInputAxis = 0.0;
            }
            
        } else if (libtrainsim::core::Helper::contains<std::string>(function,"BREAK")){
            currentInputAxis -= 0.1;
            if(abs(currentInputAxis.get()) < 0.07){
                currentInputAxis = 0.0;
            }
        }
    }  

}


