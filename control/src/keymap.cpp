#include "keymap.hpp"

#include "libtrainsim_config.hpp"
#ifdef HAS_VIDEO_SUPPORT
    #if __has_include("video.hpp")
        #include "video.hpp"
    #else
        #undef HAS_VIDEO_SUPPORT
    #endif
#endif

libtrainsim::control::keymap::keymap() {
    #ifdef HAS_VIDEO_SUPPORT
        _keymap[GDK_KEY_Escape] = "CLOSE";
        _keymap[GDK_KEY_w] = "ACCELERATE";
        _keymap[GDK_KEY_s] = "BREAK";
        _keymap[GDK_KEY_p] = "EMERGENCY_BREAK";
    #else
        _keymap[27] = "CLOSE";
        _keymap['w'] = "ACCELERATE";
        _keymap['s'] = "BREAK";
        _keymap['p'] = "EMERGENCY_BREAK";
        _keymap['\0'] = "OTHER";
    #endif
};

std::vector<unsigned int> libtrainsim::control::keymap::getKeyList(const std::string& key_name) const{
    std::vector<unsigned int> keys;
    
    for (auto x : _keymap){
        if(x.second == key_name){keys.emplace_back(x.first);};
    }
    
    return keys;
}

std::vector<std::pair<unsigned int, std::string>> libtrainsim::control::keymap::getAllKeys() const{
    std::vector<std::pair<unsigned int, std::string>> keys;
    keys.reserve(_keymap.size());
    
    for (auto x : _keymap){
        keys.emplace_back(x);
    }
    
    return keys;
}

void libtrainsim::control::keymap::add(unsigned int key, const std::string& function){
    _keymap.insert({key,function});
}

void libtrainsim::control::keymap::remove ( unsigned int key ) {
    _keymap.erase(key);
}

std::string libtrainsim::control::keymap::getFunction ( unsigned int key ) {
    
    return _keymap.count(key) > 0 ? _keymap[key] : "NONE";
}
