#include "glfwKeyTranslation.hpp"

#ifdef HAS_GLFW_SUPPORT
libtrainsim::control::glfwKeyTranslation::glfwKeyTranslation(){
    _keymap[GLFW_KEY_ESCAPE] = static_cast<char>(27);
    _keymap[GLFW_KEY_W] = 'w';
    _keymap[GLFW_KEY_S] = 's';
    _keymap[GLFW_KEY_UNKNOWN] = '\0';
}

char libtrainsim::control::glfwKeyTranslation::getKeyAsChar_impl(int glfwKey){
    return _keymap.count(glfwKey) > 0 ? _keymap[glfwKey] : '\0';
}
            
int libtrainsim::control::glfwKeyTranslation::getCharAsKey_impl(char charKey){
    for (auto x : _keymap){
        if(x.second == charKey){return x.first;};
    }
    
    return GLFW_KEY_UNKNOWN;
}
#endif
