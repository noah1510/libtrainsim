#include "VideoBackends.hpp"

libtrainsim::Video::VideoBackendDefinition::VideoBackendDefinition(WindowingBackends _win, RendererBackends _ren):windowType{_win}, rendererType{_ren}{};


bool libtrainsim::Video::VideoBackendDefinition::operator==(const libtrainsim::Video::VideoBackendDefinition& other)const{
    return windowType == other.windowType && rendererType == other.rendererType;
}

bool libtrainsim::Video::VideoBackendDefinition::operator!=(const libtrainsim::Video::VideoBackendDefinition& other)const{
    return windowType != other.windowType || rendererType != other.rendererType;
}