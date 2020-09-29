#include "format.hpp"
#include <iostream>
#include <fstream>

libtrainsim::Format::Format(){

}

std::string libtrainsim::Format::hello_impl() const{
    return "Hello from the Format Singleton!";
}

bool libtrainsim::Format::loadTrack_impl(const std::filesystem::path& URI){
    if(URI.empty()){
        std::cerr << "The file location is empty" << std::endl;
        return true;
    }

    if (URI.extension() != ".json" ){
        std::cerr << "the file has no json extention" << std::endl;
        return true;
    }

    auto in = std::ifstream(URI);

    in >> data_json;

    return false;

}