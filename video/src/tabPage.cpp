#include "tabPage.hpp"

#include "imguiHandler.hpp"

libtrainsim::Video::tabPage::tabPage(std::string name):displayText{name}{}
libtrainsim::Video::tabPage::~tabPage(){}

void libtrainsim::Video::tabPage::displayContent(){}

void libtrainsim::Video::tabPage::operator()(){
    if(ImGui::BeginTabItem(displayText.c_str())){
        this->displayContent();
        ImGui::EndTabItem();
    } 
}

std::string_view libtrainsim::Video::tabPage::getName() const {
    return displayText;
}


