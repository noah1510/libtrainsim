#include "track_data.hpp"
#include <cmath>
#include <iostream>
#include <fstream>

using namespace libtrainsim::core;
using namespace sakurajin::unit_system::base::literals;

Track_data_point::Track_data_point(uint64_t Frame, sakurajin::unit_system::base::length Location):_frame{Frame},_location{Location}{};

uint64_t Track_data_point::frame() const{
    return _frame;
}

sakurajin::unit_system::base::length Track_data_point::location() const{
    return _location;
}

Track_data::Track_data(const std::string& URI):libtrainsim::core::Track_data::Track_data (std::filesystem::path(URI)) {};
Track_data::Track_data(const char* URI):libtrainsim::core::Track_data::Track_data (std::filesystem::path(URI)) {};

Track_data::Track_data(const std::filesystem::path& URI){
    if(!std::filesystem::exists(URI)){
        std::cerr << "The Data file location is empty:" << URI.string() << std::endl;
        return;
    }

    if (URI.extension() != ".json" ){
        std::cerr << "the file has no json extention" << std::endl;
        return;
    }

    auto in = std::ifstream(URI);

    json data_json;
    in >> data_json;
    
    m_isValid = parseJsonData(data_json);
    return;
}

Track_data::Track_data(const json& data_json){    
    m_isValid = parseJsonData(data_json);
    return;
}

bool Track_data::parseJsonData(const json& data_json){
    if(data_json.size() == 0){
        std::cerr << "The array size is 0" << std::endl;
        return false;
    }
    
    data.reserve(data_json.size());
    for (auto dat:data_json){
        sakurajin::unit_system::base::length location{dat["location"].get<double>()};
        uint64_t frame = dat["frame"].get<uint64_t>();
        
        //libtrainsim::core::Track_data_point point = libtrainsim::core::Track_data_point{frame,location};
        data.emplace_back(frame,location);
    }
    
    return true;
}

Track_data::~Track_data(){}

int64_t Track_data::getFrame_c(sakurajin::unit_system::base::length location, int64_t index, int64_t lower, int64_t upper) const{
    if(!isValid() || lower < 0 || lower > getSize() || upper < lower || upper > getSize() || index < lower || index > upper){return 0;};

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

int64_t Track_data::getFrame(sakurajin::unit_system::base::length location){
    if(!isValid()){return 0;};

    return last_frame_index = getFrame_c(location,last_frame_index,0,getSize());
}

int64_t Track_data::getFrame(sakurajin::unit_system::base::length location) const{
    if(!isValid()){return 0;};

    return getFrame_c(location,getSize()/2,0,getSize());
}
            
int64_t Track_data::getSize() const{
    if(!isValid()){return 0;};
    return data.size();
}

bool Track_data::isValid() const{
    return m_isValid;
}

sakurajin::unit_system::base::length Track_data::lastLocation() const{
    if(!isValid()){return 0_m;};
    
    return data.back().location();
}
            
sakurajin::unit_system::base::length Track_data::firstLocation() const{
    if(!isValid()){return 0_m;};
    
    return data.front().location();
}
