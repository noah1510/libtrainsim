#pragma once

#include <shared_mutex>
#include <mutex>
#include <memory>
#include <chrono>
#include <cmath>

#include "track_configuration.hpp"
#include "types.hpp"

namespace libtrainsim{
    class physics{
        private:

            std::shared_mutex mutex_data;

            double acelleration = 0.0f;

            double velocity = 0.0f;

            double location = 0.0f;

            double max_velocity = 0.0f;
            
            const libtrainsim::core::Track config;
            
            std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long int, std::ratio<1, 1000000000 >>> last_update;
            
            std::shared_mutex mutex_error;
            bool hasError = true;
            
            const bool autoTick;

        public:
            
            physics(const libtrainsim::core::Track& conf, bool _autoTick = false);

            ~physics();
            
            bool isValid();
            
            double getVelocity();
            
            void setAcelleration(double acc);

            double getLocation();
            
            bool reachedEnd();
            
            void tick();
            
            static std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long int, std::ratio<1, 1000000000 >>> now(){
                return std::chrono::high_resolution_clock::now();
            }
    };
}
