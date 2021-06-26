#include "physics.hpp"

using namespace libtrainsim;
using namespace libtrainsim::core;

using namespace sakurajin::unit_system::base::literals;
using namespace sakurajin::unit_system::common::literals;
using namespace sakurajin::unit_system;

physics::physics(const Track& conf, bool _autoTick):config(conf),autoTick(_autoTick){
    if(!config.isValid()){return;};

    std::scoped_lock<std::shared_mutex> lock1(mutex_data);
    velocity = 0.0_mps;
    location = config.firstLocation();
    acelleration = 0.0_mps2;



    last_update = now();

    std::scoped_lock<std::shared_mutex> lock2(mutex_error);
    hasError = false;
    return;
};

physics::~physics(){

};

common::speed physics::getVelocity(){
    if(autoTick){tick();};
    std::shared_lock<std::shared_mutex> lock(mutex_data);
    return velocity;
}

base::length physics::getLocation(){
    if(autoTick){tick();};
    std::shared_lock<std::shared_mutex> lock(mutex_data);
    return location;
}
/*
void physics::setAcelleration(common::acceleration acc){
    tick();
    std::scoped_lock<std::shared_mutex> lock(mutex_data);
    acelleration = config.train().clampAcceleration(acc);
}*/
/*
void physics::setMass(base::mass mass){
    //get the Trainmass from Train_data.json
}

void physics::setTrainPower(common::power pow){
    tick();
    //should be set in the same way as the accelleration is set now.
}
*/

common::force physics::getTraction(){
    if(autoTick){tick();};
    std::shared_lock<std::shared_mutex> lock(mutex_data);
    return traction;
}

common::force physics::calcMaxForce(base::mass mass, common::acceleration g, long double track_drag)const{
    auto maxforce = mass*g*track_drag;
    return maxforce;
}

bool physics::isValid(){
    std::shared_lock<std::shared_mutex> lock(mutex_error);
    return !hasError;
}

bool physics::reachedEnd(){
    if(autoTick){tick();};
    std::scoped_lock<std::shared_mutex> lock(mutex_data);
    return std::abs((location - config.lastLocation()).value) < 0.1;
}

void physics::tick(){
    std::scoped_lock<std::shared_mutex> lock(mutex_data);

    auto new_time = now();

    base::time_si dt = unit_cast(new_time - last_update);

    auto mass = config.train().getMass();
    auto track_drag = config.train().getTrackDrag();

    auto MaxForce = calcMaxForce(mass,g,track_drag) ;

    ///@Todo improve calculation by considering drag.
    ///@Todo calculation of acceleration by power of Train)
    ///@Todo Fahrstufen mit clamp von -1,0 bis 1,0
    //Bremsvorgang implementieren mit Fahrstufen schalter
    //airdrag miteinarbeiten
    //Speedlevelclamp in Trainproperties.cpp
    //Bremsen
    //Traction noch genauer beschreiben/ Traktion ist noch nicht definieren
    //doxygen dokumentation für bericht unter doxygen.nl




    if(abs(velocity) < 0.07 mps) {
      velocity = 0.0 mps;
    }

    if (velocity == 0) {
      common::force currTraction = MaxForce;
    } else {
      common::force currTraction = trainpower/velocity;
    }

    if (currTraction >  MaxForce) {
      currTraction = MaxForce;
    }

    accelleration = currTraction/mass;
    velocity += acelleration * dt;
    location += velocity * dt + 0.5 * (acelleration * dt * dt);

    location = clamp(location, config.firstLocation(),config.lastLocation());
    //velocity = clamp(velocity,0_mps,config.train().getMaxVelocity());

    last_update = new_time;

    return;
}
