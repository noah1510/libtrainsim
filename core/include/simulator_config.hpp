#pragma once

#include "track_configuration.hpp"
#include "train_properties.hpp"

#include <filesystem>
#include <vector>
#include <exception>

#include <nlohmann/json.hpp>

namespace libtrainsim{
    namespace core {
        
        class simulatorConfiguration{
        private:
            std::vector<libtrainsim::core::Track> tracks;
            std::vector<libtrainsim::core::train_properties> extraTrains;
            
            std::filesystem::path serialConfigLocation;
            
            simulatorConfiguration() = delete;
            
            uint64_t currentTrack = 0;
            
        public:
            
            /**
             * @brief Create a simulatorConfiguration from a given json file.
             * @note The json file need the correct [format](@ref simulatorConfigurationFormat).
             *
             * @param URI The location of the File
             */
            simulatorConfiguration(const std::filesystem::path& URI) noexcept(false);
            
            const std::filesystem::path& getSerialConfigLocation() const noexcept;
            
            void selectTrack(uint64_t index) noexcept(false);
            uint64_t getTrackCount() const noexcept;
            const libtrainsim::core::Track& getTrack(uint64_t index) const noexcept(false);
            const libtrainsim::core::Track& getCurrentTrack() const noexcept;
        };
    }
}
