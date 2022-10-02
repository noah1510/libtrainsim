#include "control.hpp"

using namespace std::literals;

libtrainsim::control::input_handler::input_handler(const std::filesystem::path& URI) noexcept(false){
    try{
        serial = std::make_unique<serialcontrol>(URI);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error initializing the serial control"));
    }
    
    keys = libtrainsim::control::keymap();
}

libtrainsim::control::input_handler::~input_handler() {
    serial.reset();
}


libtrainsim::control::keymap& libtrainsim::control::input_handler::Keymap() noexcept {
    return keys;
}

std::vector<std::string> libtrainsim::control::input_handler::getKeyFunctions() noexcept {
    std::vector<std::string> functions;
    
    #ifdef HAS_VIDEO_SUPPORT
        std::scoped_lock<std::mutex>{ libtrainsim::Video::imguiHandler::getIOLock() };
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            ImGui_ImplSDL2_ProcessEvent(&event);

            if(event.type == SDL_QUIT){
                functions.emplace_back("TERMINATE");
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

bool libtrainsim::control::input_handler::closingFlag() noexcept {
    if(shouldClose || shouldTeminate){
        shouldClose = false;
        return true;
    }
    
    return false;;
}

bool libtrainsim::control::input_handler::emergencyFlag() const noexcept {
    return shouldEmergencyBreak;
}

void libtrainsim::control::input_handler::update() {
    #ifdef HAS_VIDEO_SUPPORT
    try{
        libtrainsim::Video::imguiHandler::errorOffThread();
    }catch(...){
        std::throw_with_nested(std::runtime_error("make sure to update the controls on the main render thread"));
    }
    #endif
    
    auto function = getKeyFunctions();
    
    if(libtrainsim::core::Helper::contains<std::string>(function,"TERMINATE")){
        shouldTeminate = true;
    }
    
    if(libtrainsim::core::Helper::contains<std::string>(function,"CLOSE")){
        shouldClose = true;
    }

    //if there is harware input update most flags from there
    if(serial->IsConnected()){
        
        shouldEmergencyBreak = serial->get_emergencyflag();
        currentInputAxis = serial->get_slvl();

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


