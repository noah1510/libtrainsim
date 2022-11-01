#include "control.hpp"

using namespace std::literals;

libtrainsim::control::input_handler::input_handler(const std::filesystem::path& URI) noexcept(false){
    try{
        serial = std::make_unique<serialcontrol>(URI);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error initializing the serial control"));
    }
    
    keys = libtrainsim::control::keymap();
    #ifdef HAS_VIDEO_SUPPORT
    auto keyList = keys.getAllKeys();
    for(auto key:keyList){
        keys.remove(key.first);
    }
    
    keys.add(ImGuiKey_Escape ,"CLOSE");
    keys.add(ImGuiKey_W ,"ACCELERATE");
    keys.add(ImGuiKey_S ,"BREAK");
    keys.add(ImGuiKey_P ,"EMERGENCY_BREAK");
    
    keys.add(ImGuiKey_GamepadBack, "CLOSE");
    keys.add(ImGuiKey_GamepadFaceUp ,"EMERGENCY_BREAK");
    #endif
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

        if(libtrainsim::Video::imguiHandler::shouldTerminate()){
            functions.emplace_back("TERMINATE");
        }
    
        auto keysToCheck = keys.getAllKeys();
        for(size_t i = 0; i < keysToCheck.size();i++){
            ImGuiKey key = static_cast<ImGuiKey>(keysToCheck[i].first);
            if(ImGui::IsKeyPressed(key)){
                functions.emplace_back(keysToCheck[i].second);
            }
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

bool libtrainsim::control::input_handler::emergencyFlag() noexcept {
    if(shouldEmergencyBreak){
        shouldEmergencyBreak = false;
        return true;
    }
    return false;
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

        #ifdef HAS_VIDEO_SUPPORT
        if((ImGui::GetIO().BackendFlags & ImGuiBackendFlags_HasGamepad) > 0){
            auto acc = ImGui::GetIO().KeysData[ImGui::GetKeyIndex(ImGuiKey_GamepadR2)].AnalogValue;
            auto dcc = ImGui::GetIO().KeysData[ImGui::GetKeyIndex(ImGuiKey_GamepadL2)].AnalogValue;
            currentInputAxis = acc - dcc;
        }else{
        #endif
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
        #ifdef HAS_VIDEO_SUPPORT
        }
        #endif
    }  

}


