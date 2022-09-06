#include "track_data.hpp"
#include <cmath>
#include <iostream>
#include <fstream>

using namespace libtrainsim::core;
using namespace sakurajin::unit_system::base;
using namespace sakurajin::unit_system::base::literals;

Track_data_point::Track_data_point(
    uint64_t _frame, 
    length _location, 
    std::optional<double> _radius, 
    std::optional<double> _slope, 
    std::optional<double> _frictionMultiplier
):
    Frame{_frame},
    Location{_location},
    Radius{_radius},
    Slope{_slope},
    FrictionMultiplier{_frictionMultiplier}
{};

uint64_t libtrainsim::core::Track_data_point::frame() const {
    return Frame;
}

std::optional<double> libtrainsim::core::Track_data_point::frictionMultiplier() const {
    return FrictionMultiplier;
}

sakurajin::unit_system::base::length libtrainsim::core::Track_data_point::location() const {
    return Location;
}

std::optional<double> libtrainsim::core::Track_data_point::radius() const {
    return Radius;
}

std::optional<double> libtrainsim::core::Track_data_point::slope() const {
    return Slope;
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
            auto slope = Helper::getOptionalJsonField<double>(dat,"slope");
            auto frictionMultiplier = Helper::getOptionalJsonField<double>(dat,"frictionMultiplier");
            auto radiusJson = Helper::getOptionalJsonField(dat,"radius");
            std::optional<double> radius = std::optional<double>{std::numeric_limits<double>::infinity()};
            if(radiusJson.has_value()){
                if(radiusJson->is_number_float()){
                    radius = std::make_optional(radiusJson->get<double>());
                }
            }
            
            libtrainsim::core::Track_data_point point{frame,location,radius,slope,frictionMultiplier};
            data.emplace_back(point);
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading track data values"));
    }
    
}

Track_data::~Track_data(){}

uint64_t Track_data::getFrame_c(length location) const{
    uint64_t index = data.size()/2;
    uint64_t lower = 0;
    uint64_t upper = data.size();

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

const libtrainsim::core::Track_data_point & libtrainsim::core::Track_data::getDataPointAt ( sakurajin::unit_system::base::length location ) const {
    return data[getFrame_c(location)];
}


uint64_t Track_data::getFrame(length location) const{
    return getDataPointAt(location).frame();
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
