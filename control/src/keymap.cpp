#include "keymap.hpp"


libtrainsim::control::keymap::keymap() {
    _keymap[static_cast<char>(27)] = "CLOSE";
    _keymap['w'] = "ACCELERATE";
    _keymap['s'] = "BREAK";
    _keymap['p'] = "EMERGENCY_BREAK";
    _keymap['\0'] = "OTHER";
};

std::vector<char> libtrainsim::control::keymap::getKeyList(const std::string& key_name) const{
    std::vector<char> keys;
    
    for (auto x : _keymap){
        if(x.second == key_name){keys.emplace_back(x.first);};
    }
    
    return keys;
}

std::vector<char> libtrainsim::control::keymap::getAllKeys() const{
    std::vector<char> keys;
    keys.reserve(_keymap.size());
    
    for (auto x : _keymap){
        keys.emplace_back(x.first);
    }
    
    return keys;
}

void libtrainsim::control::keymap::add(char key, const std::string& function){
    _keymap.insert({key,function});
}

void libtrainsim::control::keymap::remove ( char key ) {
    _keymap.erase(key);
}

std::string libtrainsim::control::keymap::getFunction ( char key ) {
    
    return _keymap.count(key) > 0 ? _keymap[key] : "NONE";
}
