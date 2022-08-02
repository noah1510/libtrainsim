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
             * @brief the location corresponding to this settings file.
             * This may be used to write changes back into the file
             */
            std::filesystem::path fileLocation;
            
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
            
            /**
             * @brief the folder that contains all of the shader files
             */
            std::filesystem::path shaderFolderLocation;
            
            /**
             * @brief indicates if the settings are allowed to write changes back into the file
             */
            bool readOnly = false;
            
        public:
            
            /**
             * @brief Create a simulatorConfiguration from a given json file.
             * @note The json file need the correct [format](@ref simulatorConfigurationFormat).
             *
             * @param URI The location of the File
             * @param lazyLoad set to true if you only want to load tracks on demand
             */
            simulatorConfiguration(const std::filesystem::path& URI, bool lazyLoad = false) noexcept(false);
            
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
            
            /**
             * @brief ensures that the track with the given index is fully loaded
             * @param index the index which should be loaded
             */
            void ensureTrack(uint64_t index) noexcept(false);
            
            /**
             * @brief returns the location of the shader folder
             * 
             * @return const std::filesystem::path& the shader folder location
             */
            const std::filesystem::path& getShaderLocation() const noexcept;
        };
    }
}
