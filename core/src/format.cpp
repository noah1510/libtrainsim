#include "format.hpp"
#include <cmath>
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

int64_t libtrainsim::Format::getFrame_impl(double location){
    int64_t lower = 0;
    int64_t higher = data_json.size();

    int64_t index = (higher+lower)/2;

    while(true){
        double loc = data_json.at(index)["location"];

        //if it is an exact match return the current index
        if (loc == location){
            return index;
        }

        //if the current location is larger adjust the upper bound, otherwise correct the lower bound.
        if(loc > location){
            higher = index;
        }else{
            lower = index;
        }

        //get the next index
        index = (higher+lower)/2;

        //if the algorithm cannot continue exit
        if(higher == index || lower == index){
            break;
        }

        
 
    }

    return index;
    
}