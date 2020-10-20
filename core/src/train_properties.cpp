#include "train_properties.hpp"
#include <exception>
#include <iostream>
#include <fstream>
#include <shared_mutex>

using namespace libtrainsim::core;

static const double airDensity = 1.2041;
static const double gravityConstant = 9.81;

train_properties::train_properties(const std::string& URI):train_properties(std::filesystem::path(URI)){};
train_properties::train_properties(const char* URI):train_properties(std::filesystem::path(URI)){};

train_properties::train_properties(const std::filesystem::path& URI){
    if(URI.empty()){
        std::cerr << "The file location is empty" << std::endl;
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
    try{
        //todo read the json into members
        if(!data_json.is_object()){
            return;
        }
        
        auto _dat = data_json["name"];
        if(!_dat.is_string()){
            return;
        }
        name = _dat.get<std::string>();
        
        _dat = data_json["mass"];
        if(!_dat.is_number_float()){
            return;
        }
        mass = _dat.get<double>();
        
        _dat = data_json["maxVelocity"];
        if(!_dat.is_number_float()){
            return;
        }
        max_velocity = _dat.get<double>();
        
        _dat = data_json["maxAcceleration"];
        if(!_dat.is_number_float()){
            return;
        }
        max_acceleration = _dat.get<double>();
        
        _dat = data_json["trackDrag"];
        if(!_dat.empty() && _dat.is_number_float()){
            track_drag = _dat.get<double>();
        }
        
        _dat = data_json["airDrag"];
        if(!_dat.empty() && _dat.is_number_float()){
            air_drag = _dat.get<double>();
        }        
        
        hasError = false;
    }catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        return;
    }
}

double train_properties::calulateDrag(double currentVelocity) const{
    return mass * gravityConstant * track_drag + 0.5 * currentVelocity * currentVelocity * airDensity * air_drag;
}

bool train_properties::isValid() const{
    return !hasError;
}

double train_properties::clampVelocity(double currentVelocity) const{
    return (currentVelocity > max_velocity) ? currentVelocity : max_velocity;
}

double train_properties::clampAcceleration(double currentAcceleration) const{
    return (currentAcceleration > max_acceleration) ? currentAcceleration : max_acceleration;
}
