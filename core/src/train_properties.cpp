#include "train_properties.hpp"
#include <iostream>
#include <fstream>
#include <shared_mutex>

using namespace libtrainsim::core;
using namespace sakurajin::unit_system;
using namespace sakurajin::unit_system::literals;

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
        mass = sakurajin::unit_system::mass{ Helper::getJsonField<double>(data_json,"mass") };
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not read mass"));
    }

    long double powerUnit = 1.0;
    try{
        auto unit = Helper::getOptionalJsonField<std::string>(data_json, "powerUnit");
        if(unit.has_value()){
            switch(Helper::stringSwitch(unit.value(),{"W","kW"})){
                case(0):
                    powerUnit = 1.0;
                    break;
                case(1):
                    powerUnit = 1000.0;
                    break;
                default:
                    powerUnit = 1.0;
                    std::cerr << "Unknown power unit given, please update config to a valid one! Falling back to W as specified in the docs." << std::endl;
                    break;
            }
        }
    }catch(...){
        std::throw_with_nested("error reading power unit");
    }

    try{
        maxPower = sakurajin::unit_system::power{ Helper::getJsonField<double>(data_json, "maxPower"), powerUnit };
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading max power"));
    }
    
    try{
        surfaceArea = sakurajin::unit_system::area{Helper::getJsonField<double>(data_json, "surfaceArea"), 1.0};
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading train surface area"));
    }
    
    try{
        numberWagons = Helper::getJsonField<unsigned int>(data_json, "numberWagons");
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading number of wagons"));
    }
    
    try{
        wagonLength = sakurajin::unit_system::length{Helper::getJsonField<double>(data_json, "wagonLength"),1.0};
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading length of wagons"));
    }
    
    try{
        driverLength = sakurajin::unit_system::length{Helper::getJsonField<double>(data_json, "driverLength"),1.0};
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading length of driver"));
    }
    
    try{
        auto _type = Helper::getOptionalJsonField<std::string>(data_json, "trainType");
        if(_type.has_value()){
            switch(Helper::stringSwitch(_type.value(),{"passenger","cargo"})){
                case(0):
                    type = trainType::passenger;
                    break;
                case(1):
                    type = trainType::cargo;
                    break;
                default:
                    type = trainType::passenger;
                    std::cerr << "Unknown train type given, please update config to a valid one! Falling back to passenger as specified in the docs." << std::endl;
                    break;
            }
        }
    }catch(...){
        std::throw_with_nested("error reading train type");
    }


}

const std::string& train_properties::getName() const{
    return name;
}

mass train_properties::getMass() const{
    return mass;
}

power train_properties::getMaxPower() const{
    return maxPower;
}

std::optional<double> train_properties::getAirDrag() const{
    return {};
}

double train_properties::getTrackDrag() const{
    return 0.2;
}


sakurajin::unit_system::length libtrainsim::core::train_properties::getDriverLength() const {
    return driverLength;
}

sakurajin::unit_system::area libtrainsim::core::train_properties::getSurfaceArea() const {
    return surfaceArea;
}

libtrainsim::core::trainType libtrainsim::core::train_properties::getTrainType() const {
    return type;
}

sakurajin::unit_system::length libtrainsim::core::train_properties::getWagonLength() const {
    return wagonLength;
}

unsigned int libtrainsim::core::train_properties::getNumberWagons() const {
    return numberWagons;
}




