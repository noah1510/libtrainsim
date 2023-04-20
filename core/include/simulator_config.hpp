#pragma once

#include "helper.hpp"
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
        class LIBTRAINSIM_EXPORT_MACRO simulatorConfiguration{
        private:
            
            /**
             * @brief the location corresponding to this settings file.
             * This may be used to write changes back into the file
             */
            std::filesystem::path fileLocation;

            /**
             * @brief the location of the application directory
             */
            std::filesystem::path appDir;

            /**
             * @brief lazy load the track data
             * If this is true the track data will be loaded on demand. Calling the ensureTrack function
             * will load the track data if it is not already loaded. If this is false the track data will
             * be loaded on construction of the simulatorConfiguration object.
             */
            bool lazyLoad = true;
            
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
             * @brief the folder that contains all of the texture files
             */
            std::filesystem::path textureFolderLocation;
            
            /**
             * @brief the folder that contains all of the extras assets
             */
            std::filesystem::path extrasLocation;

            /**
             * @brief The name of the application for the simulator application creating this object
             */
            std::string appID;
            
            /**
             * @brief indicates if the settings are allowed to write changes back into the file
             */
            bool readOnly = false;

            /**
             * @brief automatically save the state of the simulator on changes
             */
            std::atomic<bool> autosave = true;

            /**
             * @brief the main logger provided by the simulatorConfiguration
             *
             * This logger exists to provide a core logging interface across all parts of the simulator.
             * The default logging level is set by the configuration file.
             *
             * If the logging file specifies additional loggers to be used, their type and output location
             * will automatically be added to the logger. This usually results in the creation of log files
             * in addition to the command line output.
             *
             * If additional logging interfaces should be used, they can be attached to the coreLogger using
             * the addExtraLogger function of the logger class.
             */
            std::shared_ptr<SimpleGFX::logger> coreLogger = nullptr;

            /**
             * @brief the input manager provided by the simulatorConfiguration
             *
             * This input manager exists to provide a core input interface across all parts of the simulator.
             * The default input manager is set by the configuration file.
             *
             * All eventHandles and eventPoller should be attached to this input manager.
             */
            std::shared_ptr<SimpleGFX::eventManager> inputManager = nullptr;
            
        public:
            
            /**
             * @brief Create a simulatorConfiguration from a given json file.
             * @note The json file need the correct [format](@ref simulatorConfigurationFormat).
             *
             * @param URI The location of the File
             * @param lazyLoad set to true if you only want to load tracks on demand
             */
            simulatorConfiguration(const std::filesystem::path& URI, bool lazyLoad = true, const std::string& _appID = "thm.libtrainsim") noexcept(false);
            
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
             * @brief Get a track with a given index
             * 
             * This function throws std::out_out_bounds if the index is larger than the amount of tracks.
             * To get the amount of tracks use getTrackCount()
             * 
             * @note this function is for cases when changes to a track need to be made
             * usually it should be preferred to access the track as read only
             * 
             * @param index the index of the requested track
             * @return libtrainsim::core::Track& the track at the given index
             */
            libtrainsim::core::Track& getTrack(uint64_t index) noexcept(false);

            /**
             * @brief Get the currently selected track
             * 
             * use selectTrack() to change which track will be returned by this function
             * 
             * @return const libtrainsim::core::Track& the currently selected track 
             */
            const libtrainsim::core::Track& getCurrentTrack() const noexcept;
            
            /**
             * @brief return the id of the currently selected Track
             * 
             */
            uint64_t getCurrentTrackID() const noexcept;
            
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
            
            /**
             * @brief returns the location of the texture folder
             * 
             * @return const std::filesystem::path& the texture folder location
             */
            const std::filesystem::path& getTextureLocation() const noexcept;
            
            /**
             * @brief get the location where the extras assets are stored
             * 
             * @return const std::filesystem::path& the folder that contains all of the extras
             */
            const std::filesystem::path& getExtrasLocation() const noexcept;

            /**
             * @brief returns the name of the application that created this configuration
             *
             * @return const std::string& the name of the application
             */
            const std::string& getAppID() const noexcept;

            /**
             * @brief returns the common logging interface for the simulator
             * @return std::shared_ptr<SimpleGFX::logger> the logger
             */
            std::shared_ptr<SimpleGFX::logger> getLogger() noexcept;

            /**
             * @brief returns the common input manager for the simulator
             * @return std::shared_ptr<SimpleGFX::eventManager> the input manager
             */
            std::shared_ptr<SimpleGFX::eventManager> getInputManager() noexcept;

            /**
             * @brief save the state of this object into the lastLaunch file
             * @note this function will only save the configuration not underlying objects like the logger and tracks
             * @note if autosave is enabled, this function will be called automatically when a value is changed
             */
            void save();

            /**
             * @brief create a simulator configuration by reloading the last used configuration
             * @details this function will try to load the last used configuration from the lastLaunch.json file
             * That file is created when the constructor of the simulatorConfiguration is finished successfully.
             * It stores the path to the configuration file and the lazyLoad setting.
             *
             * @param appName the name of the application
             * @return std::shared_ptr<simulatorConfiguration> the configuration
             */
            static std::shared_ptr<simulatorConfiguration> loadLast(const std::string& appName){
                auto appDir = Helper::getApplicationDirectory(appName);
                auto lastLaunchFile = appDir / "lastLaunch.json";
                if (std::filesystem::exists(lastLaunchFile)) {
                    try {
                        std::ifstream file(lastLaunchFile);
                        nlohmann::json j;
                        file >> j;

                        auto filePath = std::filesystem::path{Helper::getJsonField<std::string>(j, "lastConfigFile")};
                        auto lazyLoad = Helper::getJsonField<bool>(j, "lazyLoad");

                        auto conf = std::make_shared<simulatorConfiguration>(filePath, lazyLoad, appName);

                        *conf->getLogger() << SimpleGFX::loggingLevel::detail << "Loaded last configuration from " << filePath << " with lazyLoad = " << lazyLoad << ".";

                        auto loadedTrack = Helper::getJsonField<uint64_t>(j, "loadedTrack");
                        *conf->getLogger() << SimpleGFX::loggingLevel::detail << "restoring last loaded track: " << loadedTrack << ", name: " << conf->getTrack(loadedTrack).getName() << ".";
                        conf->selectTrack(loadedTrack);

                        return conf;
                    }catch(const std::exception& e){
                        Helper::printException(e);
                        return nullptr;
                    }
                }
                return nullptr;
            }
        };
    }
}
