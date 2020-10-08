#include "train_properties.hpp"
#include <exception>
#include <iostream>
#include <fstream>
#include <shared_mutex>

static const double airDensity = 1.2041;
static const double gravityConstant = 9.81;

libtrainsim::train_properties::train_properties(const std::filesystem::path& URI){
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

    try{
        //todo read the json into members
    }catch(std::exception e){
        std::cerr << e.what() << std::endl;
        return;
    }

    hasError = false;
}

double libtrainsim::train_properties::calulateDrag(double currentVelocity) const{
    return mass * gravityConstant * track_drag + 0.5 * currentVelocity * currentVelocity * airDensity * air_drag;
}

bool libtrainsim::train_properties::isValid() const{
    return !hasError;
}

double libtrainsim::train_properties::clampVelocity(double currentVelocity) const{
    return (currentVelocity > max_velocity) ? currentVelocity : max_velocity;
}

double libtrainsim::train_properties::clampAcceleration(double currentAcceleration) const{
    return (currentAcceleration > max_acceleration) ? currentAcceleration : max_acceleration;
}