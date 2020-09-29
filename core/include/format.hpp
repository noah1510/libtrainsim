#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace libtrainsim{
    class Format{
        private:
            /**
             * @brief The constructor is private to prevent others from creating thier own object.
             * 
             */
            Format();

            /**
             * @brief This object contains the json data of the current track.
             * 
             */
            json data_json;

            /**
             * @brief Get the Instance of this singleton
             * 
             * @return Format& a reference to the instance.
             */
            static Format& getInstance(){
                static Format instance;
                return instance;
            }

            /**
             * @brief The implementation of the hello function
             * 
             * @return std::string the greeting
             */
            std::string hello_impl() const;

            /**
             * @brief This function implements loadTrack
             * 
             * @param URI The file location
             * @return true there was an error
             * @return false there was no error
             */
            bool loadTrack_impl(const std::filesystem::path& URI);

        public:
            /**
             * @brief A simple greeting to check if the singleton is working
             * 
             * @return std::string the greeting form the Fromat singleton
             */
            static std::string hello(){
                return getInstance().hello_impl();
            }

            /**
             * @brief This function loads a track file into its data storage.
             * The file needs to be a json file with the format, specified in the [format converter](https://git.thm.de/bahn-simulator/format-converter).
             * 
             * @param URI The location of the File 
             * @return true There was an error and the file is not loaded
             * @return false No error was encountered while loading the file 
             */
            static bool loadTrack(const std::filesystem::path& URI){
                return getInstance().loadTrack_impl(URI);
            }

            
    };
}