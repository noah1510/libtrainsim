#include "window.hpp"
#include "imguiHandler.hpp"

libtrainsim::Video::window::window(const std::string& _windowName, bool registerWindow): windowName{_windowName}{
    if(registerWindow){
        imguiHandler::registerWindow(this);
    }
}
libtrainsim::Video::window::~window(){
    try{
        //imguiHandler::unregisterWindow(windowName);
        imguiHandler::unregisterWindow(this);
    }catch(...){
        std::cerr << "tried to unregister a window that was not registered!" << std::endl;
    }
}

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

const std::string & libtrainsim::Video::window::getName() const {
    return windowName;
}

