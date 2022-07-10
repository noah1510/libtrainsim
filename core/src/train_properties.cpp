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

    try{
        auto str = Helper::getOptionalJsonField<std::string>(data_json, "formatVersion");
        if(str.has_value()){
            version ver = str.value();
            if(version::compare(format_version,ver) < 0){
                throw std::runtime_error(
                    "libtrainsim format version not high enough.\nneeds at least:" + 
                    format_version.print() + " but got:" + ver.print()
                );
            };
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("format version too old"));
    }
    
    try{
        name = Helper::getJsonField<std::string>(data_json,"name");
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not read name"));
    }

    try{
        mass = sakurajin::unit_system::base::mass{ Helper::getJsonField<double>(data_json,"mass") };
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not read mass"));
    }

    long double powerUnit = 1.0;
    try{
        auto unit = Helper::getOptionalJsonField<std::string>(data_json, "powerUnit");
        if(unit.has_value()){
            if(unit.value() == "kW"){
                powerUnit = 1000.0;
            }else{
                throw std::runtime_error("unknown power unit given");
            }
        }
    }catch(...){
        std::throw_with_nested("error reading power unit");
    }

    try{
        maxPower = sakurajin::unit_system::common::power{ Helper::getJsonField<double>(data_json, "maxPower"), powerUnit };
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading max power"));
    }
    
    try{
        track_drag = Helper::getJsonField<double>(data_json,"trackDrag");
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading track drag"));
    }

    try{
        air_drag = Helper::getOptionalJsonField<double>(data_json, "airDrag");
    }catch(...){
        std::throw_with_nested("error reading air drag");
    }

}

force train_properties::calulateDrag(speed currentVelocity) const{
    currentVelocity = unit_cast(currentVelocity,1);
    auto t_drag = (mass * 1_G) * track_drag;
    if(!air_drag.has_value()){
        return t_drag;
    }
    auto a_drag = 1_N * 0.5 * currentVelocity.value * currentVelocity.value * airDensity * air_drag.value();
    return t_drag + a_drag;
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

std::optional<double> train_properties::getAirDrag() const{
    return air_drag;
}

double train_properties::getTrackDrag() const{
    return track_drag;
}
