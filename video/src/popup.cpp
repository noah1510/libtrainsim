#include "popup.hpp"

#include "imguiHandler.hpp"

libtrainsim::Video::popup::popup(std::string name):displayText{name}{}
libtrainsim::Video::popup::~popup(){}

void libtrainsim::Video::popup::content(){}

void libtrainsim::Video::popup::operator()(){
    
    if(shouldOpen && !isPopupOpen){
        ImGui::OpenPopup(displayText.c_str());
        shouldOpen = false;
    }
    
    auto popupOpen = ImGui::BeginPopup(displayText.c_str());
    if(popupOpen){
        this->content();
        shouldOpen = false;
        if(shouldClose){
            ImGui::CloseCurrentPopup();
            popupOpen = false;
            popupJustClosed = true;
            shouldClose = false;
        }
        ImGui::EndPopup();
    }else{
        popupJustClosed = isPopupOpen;
        shouldClose = false;
    }
    
    isPopupOpen = popupOpen;
}

std::string_view libtrainsim::Video::popup::getName() const {
    return displayText;
}

void libtrainsim::Video::popup::open() {
    shouldOpen = true;
}

void libtrainsim::Video::popup::close() {
    shouldClose = true;
}

bool libtrainsim::Video::popup::isOpen() const {
    return isPopupOpen;
}

bool libtrainsim::Video::popup::justClosed() const {
    return popupJustClosed;
}
