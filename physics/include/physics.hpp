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

            sakurajin::unit_system::common::acceleration acceleration;

            sakurajin::unit_system::common::speed velocity;

            sakurajin::unit_system::base::length location;

            sakurajin::unit_system::common::power trainpower;

            sakurajin::unit_system::base::mass mass;

            sakurajin::unit_system::common::force traction;

            sakurajin::unit_system::common::force currTraction;

            sakurajin::unit_system::common::force maxforce;

            sakurajin::unit_system::common::power currPower;

            sakurajin::unit_system::common::power MaxPower;

            long double speedlevel = 0.0;

            long double air_drag = 0.0;

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

            void setAcelleration(sakurajin::unit_system::common::acceleration acc);

            void setSpeedlevel(core::input_axis slvl);


            sakurajin::unit_system::common::speed getVelocity();
            /**
            * @brief
            * Accelleration is not neccessary anymore beacause it will be calculated by Trainpower and Traintraktion
            */

            sakurajin::unit_system::base::length getLocation();
            /**
            * @brief
            * Trainpower is set by player input. It will replace setting the accelleration by input from player
            */
            sakurajin::unit_system::common::acceleration getAcceleration();

            sakurajin::unit_system::common::force getTraction();

            sakurajin::unit_system::common::power getCurrPower();
            
            sakurajin::unit_system::common::force calcMaxForce(sakurajin::unit_system::base::mass mass, sakurajin::unit_system::common::acceleration g, long double track_drag)const;

            sakurajin::unit_system::common::force calcDrag();
            bool reachedEnd();

            void tick();

            static std::chrono::time_point<std::chrono::high_resolution_clock> now(){
                return std::chrono::high_resolution_clock::now();
            }
    };
}
