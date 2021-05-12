#pragma once

#include <shared_mutex>
#include <mutex>
#include <memory>
#include <chrono>
#include <cmath>

#include "track_configuration.hpp"
#include "types.hpp"

#include "common.hpp"

namespace libtrainsim{
    class physics{
        private:

            std::shared_mutex mutex_data;

            sakurajin::unit_system::common::acceleration acelleration;

            sakurajin::unit_system::common::speed velocity;

            sakurajin::unit_system::base::length location;

            sakurajin::unit_system::common::speed max_velocity;
            
            const libtrainsim::core::Track config;
            
            std::chrono::time_point<std::chrono::high_resolution_clock> last_update;
            
            std::shared_mutex mutex_error;
            bool hasError = true;
            
            const bool autoTick;

        public:
            
            physics(const libtrainsim::core::Track& conf, bool _autoTick = false);

            ~physics();
            
            bool isValid();
            
            sakurajin::unit_system::common::speed getVelocity();
            
            void setAcelleration(sakurajin::unit_system::common::acceleration acc);

            sakurajin::unit_system::base::length getLocation();
            
            bool reachedEnd();
            
            void tick();
            
            static std::chrono::time_point<std::chrono::high_resolution_clock> now(){
                return std::chrono::high_resolution_clock::now();
            }
    };
}

