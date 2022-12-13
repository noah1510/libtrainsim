#include "window.hpp"
#include "imguiHandler.hpp"

libtrainsim::Video::window::window(const std::string& _windowName): windowName{_windowName}{}
libtrainsim::Video::window::~window(){}

void libtrainsim::Video::window::draw(){
    if(showWindow){
        if(beginDraw()){
            drawContent();
        }
        endDraw();
    }
}

void libtrainsim::Video::window::drawContent(){}

bool libtrainsim::Video::window::beginDraw(){
    if(closableWindow){
        return ImGui::Begin(windowName.c_str(), &showWindow, flags);
    }else{
        return ImGui::Begin(windowName.c_str(), NULL, flags);
    }
}

void libtrainsim::Video::window::endDraw(){
    ImGui::End();
}

bool libtrainsim::Video::window::isVisible() const {
    return showWindow;
}

void libtrainsim::Video::window::show() {
    showWindow = true;
}

