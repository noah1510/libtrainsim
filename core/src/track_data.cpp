#include "track_data.hpp"
#include <cmath>
#include <iostream>
#include <fstream>

using namespace libtrainsim::core;
using namespace sakurajin::unit_system::base;
using namespace sakurajin::unit_system::base::literals;

Track_data_point::Track_data_point(uint64_t Frame, length Location):_frame{Frame},_location{Location}{};

uint64_t Track_data_point::frame() const{
    return _frame;
}

length Track_data_point::location() const{
    return _location;
}

Track_data::Track_data(const std::filesystem::path& URI){
    if(!std::filesystem::exists(URI)){
        throw std::invalid_argument("The Data file location is empty:" + URI.string());
    }

    if (URI.extension() != ".json" ){
        throw std::invalid_argument("the file has no json extention");
    }

    nlohmann::json data_json;

    try{
        auto in = std::ifstream(URI);
        in >> data_json;
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error reading file into json structure"));
    }
    
    try{
        parseJsonData(data_json);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error parsing json data"));
    }
    
}

Track_data::Track_data(const nlohmann::json& data_json){    
    try{
        parseJsonData(data_json);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error parsing json data"));
    }
}

void Track_data::parseJsonData(const nlohmann::json& data_json){
    if(!data_json.is_array()){
        throw std::invalid_argument("json data is not an array");
    }
    
    if(data_json.size() == 0){
        throw std::invalid_argument("The array size is 0");
    }
    
    data.reserve(data_json.size());
    try{
        for (auto dat:data_json){
            length location{ Helper::getJsonField<double>(dat,"location")};
            auto frame = Helper::getJsonField<uint64_t>(dat, "frame");
            
            libtrainsim::core::Track_data_point point{frame,location};
            data.emplace_back(point);
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading track data values"));
    }
    
    
}

Track_data::~Track_data(){}

uint64_t Track_data::getFrame_c(length location, uint64_t index, uint64_t lower, uint64_t upper) const{
    if(
        lower > getSize() || 
        upper < lower || 
        upper > getSize() || 
        index < lower || 
        index > upper
    ){
        return 0;
        
    };

    location = sakurajin::unit_system::unit_cast(location,1);
    
    while(true){
        auto loc = data[index].location();

        //if it is an exact match return the current index
        if (loc == location){
            return index;
        }

        //if the current location is larger adjust the upper bound, otherwise correct the lower bound.
        if(loc > location){
            upper = index;
        }else{
            lower = index;
        }

        //get the next index
        index = (upper+lower)/2;

        //if the algorithm cannot continue exit
        if(upper == index || lower == index){
            break;
        }

    }

    return index;
}

uint64_t Track_data::getFrame(length location){
    return last_frame_index = getFrame_c(location,last_frame_index,0,getSize());
}

uint64_t Track_data::getFrame(length location) const{
    return getFrame_c(location,getSize()/2,0,getSize());
}
            
uint64_t Track_data::getSize() const{
    return data.size();
}

length Track_data::lastLocation() const{
    
    return data.back().location();
}
            
length Track_data::firstLocation() const{
    return data.front().location();
}
