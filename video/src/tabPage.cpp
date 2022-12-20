#include "tabPage.hpp"

#include "imguiHandler.hpp"

libtrainsim::Video::tabPage::tabPage(std::string name):displayText{name}{}
libtrainsim::Video::tabPage::~tabPage(){}

void libtrainsim::Video::tabPage::content(){}

void libtrainsim::Video::tabPage::operator()(){
    if(ImGui::BeginTabItem(displayText.c_str())){
        this->content();
        ImGui::EndTabItem();
    } 
}

std::string_view libtrainsim::Video::tabPage::getName() const {
    return displayText;
}


