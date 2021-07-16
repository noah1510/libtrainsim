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

void physics::setSpeedlevel(core::input_axis slvl){
    tick();
    std::scoped_lock<std::shared_mutex> lock(mutex_data);
    speedlevel = slvl.get();
}

common::force physics::getTraction(){
    if(autoTick){tick();};
    std::shared_lock<std::shared_mutex> lock(mutex_data);
    return currTraction;
}

common::power physics::getCurrPower(){
  if(autoTick){tick();};
  std::shared_lock<std::shared_mutex> lock(mutex_data);
  return currPower;
}

common::force physics::calcMaxForce(base::mass mass, common::acceleration g, long double track_drag)const{
    common::force maxforce = mass*g*track_drag;
    return maxforce;
}

common::force physics::calcDrag(){
  return 0_N;
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

    sakurajin::unit_system::common::power MaxPower;
    sakurajin::unit_system::common::force MaxForce;
    sakurajin::unit_system::base::mass mass;
    long double air_drag = 0.0;
    long double train_drag = 0.0;

    mass = config.train().getMass();
    train_drag = config.train().getTrackDrag();

    MaxForce = calcMaxForce(mass,1_G,train_drag);

    MaxPower = config.train().getMaxPower();

    currPower = speedlevel*MaxPower;

    if (speedlevel > 0.007)
    {
      if (abs(velocity) < 0.007_mps){
        currTraction = MaxForce;
      }else{
        currTraction = currPower/velocity;
      }

      if (currTraction >  MaxForce) {
        currTraction = MaxForce;
      }
    }else if (speedlevel < -0.007){
      currTraction = speedlevel*MaxForce;
    }else {
        currTraction = 0_N;
        if (velocity > 0.0_mps){
          currTraction = currPower/velocity;
        }
    }

    acceleration = currTraction/mass;
    velocity += acceleration * dt;
    location += velocity * dt + 0.5 * (acceleration * dt * dt);

    location = clamp(location, config.firstLocation(),config.lastLocation());
    velocity = clamp(velocity,0_mps,MaxVelocity);

    last_update = new_time;

    return;
}
