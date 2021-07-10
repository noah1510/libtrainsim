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
    acceleration = 0.0_mps2;



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

common::acceleration physics::getAcceleration(){
    if(autoTick){tick();};
      std::shared_lock<std::shared_mutex> lock(mutex_data);
      return acceleration;
}
/*
void physics::setAcelleration(common::acceleration acc){
    tick();
    std::scoped_lock<std::shared_mutex> lock(mutex_data);
    acelleration = config.train().clampAcceleration(acc);
}*/
void physics::setSpeedlevel(core::input_axis slvl){
    tick();
    std::scoped_lock<std::shared_mutex> lock(mutex_data);
    speedlevel = slvl.get();
}

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

    auto MaxForce = calcMaxForce(mass,1_G,track_drag);

    auto MaxPower = config.train().getMaxPower();


    ///@Todo improve calculation by considering drag.
    //airdrag miteinarbeiten
    //Traction noch genauer beschreiben/ Traktion ist noch nicht definieren
    //doxygen dokumentation für bericht unter doxygen.nl


    currPower = speedlevel*MaxPower;

    if (abs(speedlevel) < 0.007){
        speedlevel = 0.0;
    }

    if (speedlevel > 0.0)
    {
      if (abs(velocity) < 0.007_mps){
        currTraction = MaxForce;
      }else{
        currTraction = currPower/velocity;
      }

      if (currTraction >  MaxForce) {
        currTraction = MaxForce;
      }
    }else if (speedlevel < 0.0){
      currTraction = speedlevel*MaxForce;
    }else if (speedlevel == 0.0){
        if (velocity > 0.0_mps){
          currTraction = currPower/velocity;
        }
    }

    acceleration = currTraction/mass;
    velocity += acceleration * dt;
    location += velocity * dt + 0.5 * (acceleration * dt * dt);

    location = clamp(location, config.firstLocation(),config.lastLocation());
    velocity = clamp(velocity,0_mps,85_mps);

    last_update = new_time;

    return;
}
