#pragma once

#include <shared_mutex>
#include <mutex>
#include <memory>
#include <chrono>
#include <cmath>

#include "track_configuration.hpp"
#include "types.hpp"
#include "input_axis.hpp"
#include "helper.hpp"

#include "common.hpp"

namespace libtrainsim{
    class physics{
        private:

            std::shared_mutex mutex_data;

            /**
            * @brief The current acceleration of the train in ms^-2.
            */
            sakurajin::unit_system::common::acceleration acceleration;

            /**
            * @brief The current velocity of the train in ms^-1.
            */
            sakurajin::unit_system::common::speed velocity;

            /**
            * @brief The highest possible velocity of train.The value can be changed to any other value which makes sense.
            * In the future this can be calculated by the resistance forces of Track and Train as well as the maximum of Trainpower.
            */
            const sakurajin::unit_system::common::speed MaxVelocity{85.0};

            /**
            * @brief The current location of the train at the track.
            */
            sakurajin::unit_system::base::length location;

            /**
            * @brief The current Traction of the Train
            */
            sakurajin::unit_system::common::force currTraction;

            /**
            * @brief The current Power of the Train. This variable is calculated and therefore not a const variable
            */
            sakurajin::unit_system::common::power currPower;

            /**
            * @brief The current Speedlevel of the Train. This variable is set by playerinput, the default value is 0.0
            */
            long double speedlevel = 0.0;

            static std::chrono::time_point<std::chrono::high_resolution_clock> now(){
                return std::chrono::high_resolution_clock::now();
            }

            const libtrainsim::core::Track config;

            std::chrono::time_point<std::chrono::high_resolution_clock> last_update;

            std::shared_mutex mutex_error;
            bool hasError = true;

            const bool autoTick;

            /**
            * @brief This function calculates the maximum amount of the Force the Train can apply. This amount depends on the mass of the Train and the resistance force
            * between train and track.
            */
            sakurajin::unit_system::common::force calcMaxForce(sakurajin::unit_system::base::mass mass, sakurajin::unit_system::common::acceleration g, long double train_drag)const;

            /**
            * @brief This function calculates the whole amount of the resistance forces and adds them together. Therefore this function uses the different types of drag coefficients from the traindata and the trackdata,
            * Furthermore it uses the mass of the train as well as the location to get the train_drag coefficient correctly.
            * This function is not implemented yet and it will return 0 by using it.
            */
            sakurajin::unit_system::common::force calcDrag();
            
            //true if the emergency break is activated.
            //the train has to break full until it is stopped, after that it is allowed to accellerate again.
            bool isEmergencyBreaking = false;

        public:

            physics(const libtrainsim::core::Track& conf, bool _autoTick = false);

            ~physics();

            bool isValid();

            /**
            * @brief This function returns the current speedlevel of the train, which is set through the actions of a player.
            * If autoTick is true, this function will automatically call tick before returning the value.
            */
            void setSpeedlevel(core::input_axis slvl);

            /**
            * @brief This function returns the current velocity of the train. If autoTick is true, this function will automatically call tick before returning the value.
            */
            sakurajin::unit_system::common::speed getVelocity();

            /**
            * @brief This function returns the current location of the train along the track.
            * If autoTick is true, this function will automatically call tick before returning the value.
            */
            sakurajin::unit_system::base::length getLocation();

            /**
            * @brief This function returns the current acceleration of the train. If autoTick is true, this function will automatically call tick before returning the value.
            */
            sakurajin::unit_system::common::acceleration getAcceleration();

            /**
            * @brief This function returns the current traction of the train. If autoTick is true, this function will automatically call tick before returning the value.
            */
            sakurajin::unit_system::common::force getTraction();

            /**
            * @brief This function returns the current power of the train. If autoTick is true, this function will automatically call tick before returning the value.
            */
            sakurajin::unit_system::common::power getCurrPower();

            /**
             * @brief activate the emergency break
             * 
             * This causes the train to break until it comes to a stop.
             */
            void emergencyBreak();
            
            bool reachedEnd();

            /**
            * @brief This Function does the whole mathematics behind the physics. It calculates the current values of location, velocity, acceleration, traction and trainpower for every tick the simulation is running.
            */
            void tick();
    };
}
