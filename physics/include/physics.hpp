#pragma once

#include <shared_mutex>
#include <mutex>
#include <memory>
#include <chrono>
#include <cmath>

#include "track_configuration.hpp"
#include "types.hpp"
#include "input_axis.hpp"

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
            * @brief The current Power of the Train. This variable is calculated and therefore not a const variable
            */
            sakurajin::unit_system::common::power trainpower;

            /**
            * @brief The mass of the Train. A const variable because its a property of the train.
            */
            sakurajin::unit_system::base::mass mass;

            /**
            * @brief The current Traction of the Train
            */
            sakurajin::unit_system::common::force currTraction;

            /**
            * @brief The maximum of Traction the Train can have on the track
            */
            sakurajin::unit_system::common::force maxforce;

            /**
            * @brief The current Power of the Train
            */
            sakurajin::unit_system::common::power currPower;

            /**
            * @brief The maximum of Power of the Train.
            */
            sakurajin::unit_system::common::power MaxPower;

            /**
            * @brief The current Speedlevel of the Train. This variable is set by playerinput, the default value is 0.0
            */
            long double speedlevel = 0.0;

            /**
            * @brief The value of the Drag caused by the air. Default value is 0.0. This variable is not used yet.
            */
            long double air_drag = 0.0;

            /**
            * @brief The value of the Drag caused by the track itself. Default value is 0.0, current value could be found in the train_properties
            */
            long double track_drag = 0.0;

            const libtrainsim::core::Track config;

            std::chrono::time_point<std::chrono::high_resolution_clock> last_update;

            std::shared_mutex mutex_error;
            bool hasError = true;

            const bool autoTick;

        public:

            physics(const libtrainsim::core::Track& conf, bool _autoTick = false);

            ~physics();

            bool isValid();

            /**
            * @brief This function sets the speedlevel variable readable, to use the value in other classes
            */
            void setSpeedlevel(core::input_axis slvl);

            /**
            * @brief This function sets the velocity variable readable for using its value in other classes
            */
            sakurajin::unit_system::common::speed getVelocity();

            /**
            * @brief This function sets the location variable readable for using its value in other classes
            */
            sakurajin::unit_system::base::length getLocation();

            /**
            * @brief This function sets the acceleration variable readable for using its value in other classes
            */
            sakurajin::unit_system::common::acceleration getAcceleration();

            /**
            * @brief This function sets the Traction variable readable for using its value in other classes
            */
            sakurajin::unit_system::common::force getTraction();

            /**
            * @brief This function sets the currentPower variable readable for using its value in other classes
            */
            sakurajin::unit_system::common::power getCurrPower();

            /**
            * @brief This function calculates the maximum amount of the Force the Train can apply. This amount depends on the mass of the Train and the resistanceforce
            * between train and track
            */
            sakurajin::unit_system::common::force calcMaxForce(sakurajin::unit_system::base::mass mass, sakurajin::unit_system::common::acceleration g, long double track_drag)const;

            /**
            * @brief This Function calulates the whole amount of the drag. It adds all types of Drags and resistance forces to calculate one value of the resistance forces.
            * This function is not implemented yet and it will return 0 by using it.
            */
            sakurajin::unit_system::common::force calcDrag();

            bool reachedEnd();

            /**
            * @brief This Function does the whole mathematics behind the physics. It calculates the current values of location, velocity, acceleration, traction and trainpower for every tick the simulation is running.
            */
            void tick();

            static std::chrono::time_point<std::chrono::high_resolution_clock> now(){
                return std::chrono::high_resolution_clock::now();
            }
    };
}
