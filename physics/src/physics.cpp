#include "physics.hpp"

using namespace libtrainsim;
using namespace libtrainsim::core;

using namespace sakurajin::unit_system::literals;
using namespace sakurajin::unit_system;

physics::physics(const Track& conf, bool _autoTick):config(conf),autoTick(_autoTick){

    std::scoped_lock<std::shared_mutex> lock1(mutex_data);
    velocity = 0.0_mps;
    location = config.firstLocation();
    current_acceleration = 0.0_mps2;



    last_update = Helper::now();

    std::scoped_lock<std::shared_mutex> lock2(mutex_error);
    hasError = false;
    return;
};

physics::~physics(){

};

void physics::emergencyBreak(){
    std::scoped_lock<std::shared_mutex> lock(mutex_data);
    isEmergencyBreaking = true;
}

speed physics::getVelocity(){
    if(autoTick){tick();};
    std::shared_lock<std::shared_mutex> lock(mutex_data);
    return velocity;
}

length physics::getLocation(){
    if(autoTick){tick();};
    std::shared_lock<std::shared_mutex> lock(mutex_data);
    return location;
}

acceleration physics::getAcceleration(){
    if(autoTick){tick();};
      std::shared_lock<std::shared_mutex> lock(mutex_data);
      return current_acceleration;
}

void physics::setSpeedlevel(core::input_axis slvl){
    tick();
    std::scoped_lock<std::shared_mutex> lock(mutex_data);
    speedlevel = slvl.get();
}

force physics::getTraction(){
    if(autoTick){tick();};
    std::shared_lock<std::shared_mutex> lock(mutex_data);
    return currTraction;
}

power physics::getCurrPower(){
  if(autoTick){tick();};
  std::shared_lock<std::shared_mutex> lock(mutex_data);
  return currPower;
}

force physics::calcMaxForce(mass mass, acceleration g, long double train_drag)const{
    force maxforce = mass*g*train_drag;
    return maxforce;
}

force physics::calcDrag(){
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

    time_si dt = unit_cast(new_time - last_update);

    //all Variables needed to caclulate the physics
    sakurajin::unit_system::power MaxPower;
    sakurajin::unit_system::force MaxForce;
    sakurajin::unit_system::mass mass;
    long double air_drag = 0.0;
    long double train_drag = 0.0;

    //defining the needed variables
    mass = config.train().getMass();
    train_drag = config.train().getTrackDrag();

    MaxForce = calcMaxForce(mass,1_G,train_drag);
    MaxPower = config.train().getMaxPower();

    if(isEmergencyBreaking){
      speedlevel = -1.0;
      if(velocity < 0.007_mps){
        isEmergencyBreaking = false;
      }
    }
    
    currPower = speedlevel*MaxPower;

    //Handling the different possibilities for Speedlevel
    //only calculating the current Force
    if (speedlevel > 0.007){
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

    //calculating parameters of movement by current Traction
    current_acceleration = currTraction/mass;
    velocity += current_acceleration * dt;
    location += velocity * dt + 0.5 * (current_acceleration * dt * dt);

    location = clamp(location, config.firstLocation(),config.lastLocation());
    velocity = clamp(velocity,0_mps,MaxVelocity);

    last_update = new_time;

    return;
}
