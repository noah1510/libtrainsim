#include "physics.hpp"

using namespace libtrainsim;
using namespace libtrainsim::core;

physics::physics(const Track& conf, bool _autoTick):config(conf),autoTick(_autoTick){
    if(!config.isValid()){return;};
    
    std::scoped_lock<std::shared_mutex> lock1(mutex_data);
    velocity = 0.0;
    location = config.firstLocation();
    acelleration = 0.0;
    
    last_update = now();
    
    std::scoped_lock<std::shared_mutex> lock2(mutex_error);
    hasError = false;
    return;
};

physics::~physics(){

};

double physics::getVelocity(){
    if(autoTick){tick();};
    std::shared_lock<std::shared_mutex> lock(mutex_data);
    return velocity;
}

double physics::getLocation(){
    if(autoTick){tick();};
    std::shared_lock<std::shared_mutex> lock(mutex_data);
    return location;
}

void physics::setAcelleration(double acc){
    tick();
    std::scoped_lock<std::shared_mutex> lock(mutex_data);
    acelleration = config.train().clampAcceleration(acc);
}

bool physics::isValid(){
    std::shared_lock<std::shared_mutex> lock(mutex_error);
    return !hasError;
}

bool physics::reachedEnd(){
    if(autoTick){tick();};
    std::scoped_lock<std::shared_mutex> lock(mutex_data);
    return std::abs(location - config.lastLocation()) < 0.1;
}

void physics::tick(){
    std::scoped_lock<std::shared_mutex> lock(mutex_data);
    
    auto new_time = now();
    auto t = std::chrono::duration_cast<std::chrono::nanoseconds>(new_time - last_update);
    
    double dt = static_cast<double>(t.count()) / 1000000000.0;
    
    ///@Todo improve calculation by considering drag.
    location += velocity * dt + 0.5 * acelleration * dt * dt;
    velocity += acelleration * dt;
    
    location = std::clamp(location, config.firstLocation(),config.lastLocation());
    velocity = std::clamp(velocity,0.0,config.train().getMaxVelocity());

    last_update = new_time;
    
    return;
}
