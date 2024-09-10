#include "physics.hpp"

using namespace sakurajin::unit_system::literals;
using namespace sakurajin::unit_system;

libtrainsim::physics::physics(const libtrainsim::core::Track& conf, bool _autoTick)
    : config(conf),
      autoTick(_autoTick) {

    velocity             = 0.0_mps;
    location             = config.firstLocation();
    current_acceleration = 0.0_mps2;
    last_update          = SimpleGFX::chrono::now();

    hasError = false;
}

libtrainsim::physics::~physics() = default;

void libtrainsim::physics::doAutoTick() {
    if (autoTick) {
        tick();
    }
}

void libtrainsim::physics::emergencyBreak() {
    isEmergencyBreaking = true;
}

bool libtrainsim::physics::emergencyBreaking(){
    return isEmergencyBreaking;
}

speed libtrainsim::physics::getVelocity() {
    doAutoTick();
    return velocity.load();
}

length libtrainsim::physics::getLocation() {
    doAutoTick();
    return location.load();
}

acceleration libtrainsim::physics::getAcceleration() {
    doAutoTick();
    return current_acceleration.load();
}

void libtrainsim::physics::setSpeedlevel(const core::input_axis& slvl) {
    doAutoTick();
    speedlevel = slvl.get();
}

force libtrainsim::physics::getTraction() {
    doAutoTick();
    return currTraction.load();
}

power libtrainsim::physics::getCurrPower() {
    doAutoTick();
    return currPower.load();
}

force libtrainsim::physics::calcMaxForce(mass mass, acceleration g, long double train_drag) const {
    return mass * g * train_drag;
}

force libtrainsim::physics::calcDrag() {
    return 0_N;
}

bool libtrainsim::physics::isValid() {
    return !hasError;
}

bool libtrainsim::physics::reachedEnd() {
    doAutoTick();
    return std::abs((location.load() - config.lastLocation()).val()) < 0.1;
}

void libtrainsim::physics::tick() {
    static std::atomic<bool> is_ticking = false;
    if (is_ticking) {
        return;
    }

    is_ticking = true;

    auto new_time = SimpleGFX::chrono::now();
    time_si dt = unit_cast(new_time - last_update.load());
    last_update = new_time;

    // all Variables needed to caclulate the physics
    long double                   air_drag   = 0.0;
    long double                   train_drag = 0.0;
    auto current_slvel = speedlevel.load();

    // defining the needed variables
    auto mass       = config.train().getMass();
    train_drag = 0.2 * config.get_frictionMultiplier(location);

    auto MaxForce = calcMaxForce(mass, 1_G, train_drag);
    auto MaxPower = config.train().getMaxPower();

    auto current_velocity = velocity.load();

    if (isEmergencyBreaking) {
        current_slvel = -1.0;
        if (current_velocity < 0.007_mps) {
            isEmergencyBreaking = false;
        }
    }

    currPower = current_slvel * MaxPower;

    // Handling the different possibilities for current_slvel
    // only calculating the current Force
    if (current_slvel > 0.007) {
        if (std::abs(current_velocity) < 0.007_mps) {
            currTraction = MaxForce;
        } else {
            currTraction = currPower.load() / current_velocity;
        }

        if (currTraction > MaxForce) {
            currTraction = MaxForce;
        }
    } else if (current_slvel < -0.007) {
        currTraction = current_slvel * MaxForce;
    } else {
        currTraction = 0_N;
        if (current_velocity > 0.0_mps) {
            currTraction = currPower.load() / current_velocity;
        }
    }

    // calculating parameters of movement by current Traction
    current_acceleration = currTraction.load() / mass;
    current_velocity += current_acceleration.load() * dt;

    auto location_delta = current_velocity * dt + 0.5 * (current_acceleration.load() * dt * dt);

    location = clamp(location.load() + location_delta, config.firstLocation(), config.lastLocation());
    velocity = clamp(current_velocity, 0_mps, MaxVelocity);

    is_ticking = false;
}
