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

train_properties::train_properties(const std::string& URI):train_properties(std::filesystem::path(URI)){};
train_properties::train_properties(const char* URI):train_properties(std::filesystem::path(URI)){};

train_properties::train_properties(const std::filesystem::path& URI){
    if(!std::filesystem::exists(URI)){
        std::cerr << "The Train file location is empty:" << URI.string() << std::endl;
        return;
    }

    if (URI.extension() != ".json" ){
        std::cerr << "the file has no json extention" << std::endl;
        return;
    }

    auto in = std::ifstream(URI);

    in >> data_json;

    loadJsonData();
}

train_properties::train_properties(const json& data){
    data_json = data;

    loadJsonData();
}

void train_properties::loadJsonData(){
    if(!data_json.is_object()){
        return;
    }

    auto _dat = data_json["formatVersion"];
    if(!_dat.empty() && _dat.is_string()){
        version ver = _dat.get<std::string>();
        if(version::compare(format_version,ver) < 0){
            std::cerr << "libtrainsim format version not high enough." << std::endl;
            std::cerr << "needs at least:" << format_version.print() << " but got:" << format_version.print() << std::endl;
            return;
        };
    };

    _dat = data_json["name"];
    if(!_dat.is_string()){
        std::cerr << "name is not a string" << std::endl;
        return;
    }
    name = _dat.get<std::string>();

    _dat = data_json["mass"];
    if(!_dat.is_number_float()){
        std::cerr << "mass is not a float" << std::endl;
        return;
    }
    mass = base::mass{_dat.get<double>()};

    long double powerUnit = 1.0;
    _dat = data_json["powerUnit"];
    if(!_dat.empty() && _dat.is_string()){
        auto unit = _dat.get<std::string>();
        if(unit == "kW"){
            powerUnit = 1000.0;
        }
    }

    _dat = data_json["maxPower"];
    if(!_dat.is_number_float()){
        std::cerr << "Power is not a float" << std::endl;
        return;
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
