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

            /**
             * @brief implements getFrame
             * 
             * @param location the location on the track in meters
             * @return int64_t the nearest frame to that location
             */
            int64_t getFrame_impl(double location);

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

            /**
             * @brief Get the Frame to the given location.
             * Because a binary search is used to find the frame and the location is a floating point value, the returned Frame might not be the nearest.
             * 
             * @param location the location on the track in meters
             * @return int64_t the nearest frame to that location
             */
            static int64_t getFrame(double location){
                return getInstance().getFrame_impl(location);
            }

            /**
             * @brief dump the data of the json content as a return value
             * 
             * @param ident If indent is nonnegative, then array elements and object members will be pretty-printed with that indent level. An indent level of 0 will only insert newlines. -1 (the default) selects the most compact representation.
             * @return std::string the data of the loaded json file
             */
            static std::string dump(int ident = -1){
                return getInstance().data_json.dump(ident);
            }

            /**
             * @brief Get the number of elements in the loaded json file
             * 
             * @return int64_t the number of elements
             */
            static int64_t getSize(){
                return getInstance().data_json.size();
            }

            
    };
}