#include "popup.hpp"

#include "imguiHandler.hpp"

libtrainsim::Video::popup::popup(std::string name):displayText{name}{}
libtrainsim::Video::popup::~popup(){}

void libtrainsim::Video::popup::content(){}

void libtrainsim::Video::popup::operator()(){
    
    if(ImGui::BeginPopup(displayText.c_str())){
        this->content();
        ImGui::EndPopup();
    } 
}

std::string_view libtrainsim::Video::popup::getName() const {
    return displayText;
}

void libtrainsim::Video::popup::open() {
    ImGui::OpenPopup(displayText.c_str());
}
