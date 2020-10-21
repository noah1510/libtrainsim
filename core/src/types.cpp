#include "types.hpp"

std::vector<std::string> libtrainsim::core::split_string(const std::string& s, char delimiter){
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)){
        tokens.push_back(token);
    }
    return tokens;
}
    
libtrainsim::core::version::version(uint64_t x, uint64_t y, uint64_t z):major{x},minor{y},patch{z}{};

libtrainsim::core::version::version(const std::string& ver):version(
    std::stoi(split_string(ver, '.').at(0)),
    std::stoi(split_string(ver, '.').at(1)),
    std::stoi(split_string(ver, '.').at(2))
){};

std::string libtrainsim::core::version::print() const{
    return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}
