#pragma once

#include "track_configuration.hpp"
#include "train_properties.hpp"

#include <filesystem>
#include <vector>
#include <exception>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace libtrainsim{
    namespace core {
        
        class simulatorConfiguration{
        private:
            std::vector<libtrainsim::core::Track> tracks;
            std::vector<libtrainsim::core::train_properties> extraTrains;
            
            std::filesystem::path serialConfigLocation;
            
        public:
            
            /**
             * @brief Create a simulatorConfiguration from a given json file.
             * @note The json file need the correct [format](@ref simulatorConfigurationFormat).
             *
             * @param URI The location of the File
             */
            simulatorConfiguration(const std::filesystem::path& URI);
        };
    }
}
