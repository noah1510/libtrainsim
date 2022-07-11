#pragma once

#include "track_configuration.hpp"
#include "train_properties.hpp"

#include <filesystem>
#include <vector>
#include <exception>
#include <thread>
#include <future>

#include <nlohmann/json.hpp>

namespace libtrainsim{
    namespace core {
        /**
         * @brief A general configuration class for the simulator
         * 
         */
        class simulatorConfiguration{
        private:
            /**
             * @brief An array containing all of the loaded tracks
             * 
             */
            std::vector<libtrainsim::core::Track> tracks;

            /**
             * @brief an array containing all of the loaded trains
             * 
             */
            std::vector<libtrainsim::core::train_properties> extraTrains;
            
            /**
             * @brief the location of the serial configuration file
             * 
             */
            std::filesystem::path serialConfigLocation;
            
            /**
             * @brief remove the default constructor to force a file to be given
             * 
             */
            simulatorConfiguration() = delete;
            
            /**
             * @brief the index of the currently selected track
             * 
             */
            uint64_t currentTrack = 0;
            
        public:
            
            /**
             * @brief Create a simulatorConfiguration from a given json file.
             * @note The json file need the correct [format](@ref simulatorConfigurationFormat).
             *
             * @param URI The location of the File
             */
            simulatorConfiguration(const std::filesystem::path& URI) noexcept(false);
            
            /**
             * @brief Get the location of the serial config settings file
             * 
             * @return const std::filesystem::path& the path to serial config file
             */
            const std::filesystem::path& getSerialConfigLocation() const noexcept;
            
            /**
             * @brief change which track is used as default track
             * 
             * This function throws std::out_out_bounds if the index is larger than the amount of tracks.
             * To get the amount of tracks use getTrackCount()
             * 
             * @param index the index of the new track
             */
            void selectTrack(uint64_t index) noexcept(false);

            /**
             * @brief get the number of tracks that were loaded from the settings
             * 
             * @return uint64_t the number of available tracks
             */
            uint64_t getTrackCount() const noexcept;

            /**
             * @brief Get a track with a given index
             * 
             * This function throws std::out_out_bounds if the index is larger than the amount of tracks.
             * To get the amount of tracks use getTrackCount()
             * 
             * @param index the index of the requested track
             * @return const libtrainsim::core::Track& the track at the given index
             */
            const libtrainsim::core::Track& getTrack(uint64_t index) const noexcept(false);

            /**
             * @brief Get the currently selected track
             * 
             * use selectTrack() to change which track will be returned by this function
             * 
             * @return const libtrainsim::core::Track& the currently selected track 
             */
            const libtrainsim::core::Track& getCurrentTrack() const noexcept;
        };
    }
}
