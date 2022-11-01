#include "keymap.hpp"


libtrainsim::control::keymap::keymap() {
    _keymap[27] = "CLOSE";
    _keymap['w'] = "ACCELERATE";
    _keymap['s'] = "BREAK";
    _keymap['p'] = "EMERGENCY_BREAK";
    _keymap['\0'] = "OTHER";
};

std::vector<int> libtrainsim::control::keymap::getKeyList(const std::string& key_name) const{
    std::vector<int> keys;
    
    for (auto x : _keymap){
        if(x.second == key_name){keys.emplace_back(x.first);};
    }
    
    return keys;
}

std::vector<std::pair<int, std::string>> libtrainsim::control::keymap::getAllKeys() const{
    std::vector<std::pair<int, std::string>> keys;
    keys.reserve(_keymap.size());
    
    for (auto x : _keymap){
        keys.emplace_back(x);
    }
    
    return keys;
}

void libtrainsim::control::keymap::add(int key, const std::string& function){
    _keymap.insert({key,function});
}

void libtrainsim::control::keymap::remove ( int key ) {
    _keymap.erase(key);
}

std::string libtrainsim::control::keymap::getFunction ( int key ) {
    
    return _keymap.count(key) > 0 ? _keymap[key] : "NONE";
}
