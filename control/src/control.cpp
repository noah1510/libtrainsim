#include "control.hpp"

using namespace std::literals;

libtrainsim::control::input_handler::input_handler(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _conf) noexcept(false): conf{_conf}{
    keys = libtrainsim::control::keymap();

    #ifdef HAS_VIDEO_SUPPORT

    keyboardController = Gtk::EventControllerKey::create();
    keyboardController->signal_key_pressed().connect([this](guint keyval, guint, Gdk::ModifierType){
        auto keysToCheck = keys.getAllKeys();
        for (auto key:keysToCheck){
            if(key.first == keyval){
                //call all callbacks and exit once the event is handled
                for(auto call:eventCallbacks){
                    if(std::get<0>(call)(key.second)){
                        return true;
                    }
                }
                if(key.second == "CLOSE" || key.second == "TERMINATE"){
                    return true;
                }
            }
        }

        return false;
    },false);

    addEventCallback(sigc::mem_fun(*this, &input_handler::updateKeybord),0);
    //keys.add(ImGuiKey_GamepadBack, "CLOSE");
    //keys.add(ImGuiKey_GamepadFaceUp ,"EMERGENCY_BREAK");
    #endif
}

void libtrainsim::control::input_handler::addEventCallback(std::function<bool (std::string)> callback, int priority){

    for(auto i = eventCallbacks.begin(); i < eventCallbacks.end();i++){
        auto [_, prio] = *i;
        if(prio > priority){
            eventCallbacks.insert(i, {callback, priority});
            return;
        }
    }
    eventCallbacks.emplace_back(std::tuple<std::function<bool (std::string)>, int>{callback, priority});
}


libtrainsim::control::input_handler::~input_handler() {
    resetFlags();
}

void libtrainsim::control::input_handler::resetFlags(){
    serial.reset();

    currentInputAxis = 0.0;
    shouldClose = false;
    shouldTeminate = false;
    shouldEmergencyBreak = false;

    running = false;
}

void libtrainsim::control::input_handler::startSimulation(){

    try{
        //create a serial controller and if it cannot connect destroy it
        serial = std::make_unique<serialcontrol>(conf->getSerialConfigLocation());
        if(!serial->IsConnected()){
            serial.reset();
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error initializing the serial control"));
    }


    running = true;
}



#ifdef HAS_VIDEO_SUPPORT
void libtrainsim::control::input_handler::registerWindow(Gtk::Window& win){
    win.add_controller(keyboardController);
}

#endif


libtrainsim::control::keymap& libtrainsim::control::input_handler::Keymap() noexcept {
    return keys;
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

bool libtrainsim::control::input_handler::updateKeybord(std::string eventName){

    switch(libtrainsim::core::Helper::stringSwitch(eventName, {
        "TERMINATE",
        "CLOSE",
        "EMERGENCY_BREAK",
        "ACCELERATE",
        "BREAK"
    })){
        case(0):
            shouldTeminate = true;
            return false;
        case(1):
            shouldClose = true;
            return false;
        case(2):
            if(!running){return false;};
            shouldEmergencyBreak = true;
            return true;
        case(3):
            if(!running){return false;};
            currentInputAxis += 0.1;
            if(abs(currentInputAxis.get()) < 0.07){
                currentInputAxis = 0.0;
            }
            return true;
        case(4):
            if(!running){return false;};
            currentInputAxis -= 0.1;
            if(abs(currentInputAxis.get()) < 0.07){
                currentInputAxis = 0.0;
            }
            return true;
        default:
            return false;

    }
}


void libtrainsim::control::input_handler::update() {
    //if there is harware input update most flags from there
    if(serial->IsConnected()){
        shouldEmergencyBreak = serial->get_emergencyflag();
        currentInputAxis = serial->get_slvl();
    }
    /*else{
        #ifdef HAS_VIDEO_SUPPORT
        if((ImGui::GetIO().BackendFlags & ImGuiBackendFlags_HasGamepad) > 0){
            auto acc = ImGui::GetIO().KeysData[ImGui::GetKeyIndex(ImGuiKey_GamepadR2)].AnalogValue;
            auto dcc = ImGui::GetIO().KeysData[ImGui::GetKeyIndex(ImGuiKey_GamepadL2)].AnalogValue;
            currentInputAxis = acc - dcc;
        }
        #endif
    } */

}


