#include "train_properties.hpp"
#include <iostream>
#include <fstream>
#include <shared_mutex>

using namespace libtrainsim::core;
using namespace sakurajin::unit_system::common;
using namespace sakurajin::unit_system::common::literals;
using namespace sakurajin::unit_system;
using namespace sakurajin::unit_system::base::literals;

static const long double airDensity = 1.2041;

train_properties::train_properties(const std::filesystem::path& URI){
    if(!std::filesystem::exists(URI)){
        throw std::invalid_argument("The Train file location is empty:" + URI.string());
    }

    if (URI.extension() != ".json" ){
        throw std::invalid_argument("the file has no json extention");
    }

    nlohmann::json data_json;
    
    try{
        auto in = std::ifstream(URI);
        in >> data_json;
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading the file into json structure"));
    }

    try{
        loadJsonData(data_json);
    }catch(...){
        std::throw_with_nested(std::runtime_error("error parsing the json data"));
    }
}

train_properties::train_properties(const nlohmann::json& data){
    try{
        loadJsonData(data);
    }catch(...){
        std::throw_with_nested(std::runtime_error("error parsing the json data"));
    }
}

void train_properties::loadJsonData(const nlohmann::json& data_json){
    if(!data_json.is_object()){
        throw std::invalid_argument("json data is not an object");
    }

    auto _dat = data_json["formatVersion"];
    if(!_dat.empty() && _dat.is_string()){
        version ver = _dat.get<std::string>();
        if(version::compare(format_version,ver) < 0){
            throw std::runtime_error(
                "libtrainsim format version not high enough.\nneeds at least:" + 
                format_version.print() + " but got:" + format_version.print()
            );
        };
    };

    _dat = data_json["name"];
    if(!_dat.is_string()){
        throw std::runtime_error("name is not a string");
    }
    name = _dat.get<std::string>();

    _dat = data_json["mass"];
    if(!_dat.is_number_float()){
        throw std::runtime_error("mass is not a float");
    }
    mass = base::mass{_dat.get<double>()};

    long double powerUnit = 1.0;
    _dat = data_json["powerUnit"];
    if(!_dat.empty() && _dat.is_string()){
        auto unit = _dat.get<std::string>();
        if(unit == "kW"){
            powerUnit = 1000.0;
        }else{
            throw std::runtime_error("unknown power unit given");
        }
    }

    _dat = data_json["maxPower"];
    if(!_dat.is_number_float()){
        throw std::runtime_error("power is not a float");
    }

    maxPower = common::power{_dat.get<double>(),powerUnit};

    _dat = data_json["trackDrag"];
    if(!_dat.empty() && _dat.is_number_float()){
        track_drag = _dat.get<double>();
    }

    _dat = data_json["airDrag"];
    if(!_dat.empty() && _dat.is_number_float()){
        air_drag = _dat.get<double>();
    }

    hasError = false;
}

force train_properties::calulateDrag(speed currentVelocity) const{
    currentVelocity = unit_cast(currentVelocity,1);
    return (mass * 1_G) * track_drag + 1_N * 0.5 * currentVelocity.value * currentVelocity.value * airDensity * air_drag;
}

bool train_properties::isValid() const{
    return !hasError;
}

const std::string& train_properties::getName() const{
    return name;
}

base::mass train_properties::getMass() const{
    return mass;
}

common::power train_properties::getMaxPower() const{
    return maxPower;
}

long double train_properties::getAirDrag() const{
    return air_drag;
}

long double train_properties::getTrackDrag() const{
    return track_drag;
}
