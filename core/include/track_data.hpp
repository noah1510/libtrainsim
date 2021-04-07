#pragma once

/**
 * @file track_data.hpp
 * @author Noah Kirschmann (noah.kirschmann@mnd.thm.de)
 * @brief This file contains the definition of the Track data to manage tracks data.
 * @version 0.4.0
 * @date 2020-10-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <filesystem>
#include <algorithm>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace libtrainsim {
    namespace core {
        
        /**
         * @brief This class stores the data of a single data point from the track data.
         * For the format of the json files look [here](@ref track_data_format).
         */
        class Track_data_point {
        private:
            
            /**
             * @brief The frame number of the data point
             */
            uint64_t _frame;
            
            /**
             * @brief The location along the track in m.
             */
            double _location;
            
        public:
            
            /**
             * @brief The frame number of the data point
             */
            uint64_t frame() const;
            
            /**
             * @brief The location along the track in m.
             */
            double location() const;
            
            /**
             * @brief Create the minimum required Data point without any optional value set.
             */
            Track_data_point(uint64_t Frame, double Location);
            
        };
        
        /**
         * @brief This class stores the data of a given track.
         * For the format of the json files look [here](@ref track_data_format).
         * @todo check if the correct format is actually used to prevent crashes.
         */
        class Track_data {
        private:
            /**
             * @brief The parsed data of the current Track.
             * 
             */
            std::vector<Track_data_point> data;

            /**
             * @brief This saves the last value returned by getFrame to speed the binary search up.
             *
             */
            int64_t last_frame_index = 0;

            /**
             * @brief is true when the json file was loaded correctly
             *
             */
            bool m_isValid = false;
            
            /**
             * @brief Parses the track data json format into the data array.
             * 
             * @return bool true The parsed data was valid
             * @return bool false The parsed data was not valid
             */
            bool parseJsonData(const json& data_json);

            /**
             * @brief this is the binary search to find the frame of a given location, between the lower and upper bound with the a given starting index.
             * This algorithm might not return what is actually the nearest frame to the wanted value.
             * Since a binary search is performed the second closest value might be the one that will be returned.
             *
             * @param location the location on the track in meters
             * @param index the first index to be checked
             * @param lower the lower search bound
             * @param upper the upper serch bound
             * @return int64_t the nearest frame to that location
             */
            int64_t getFrame_c(double location, int64_t index, int64_t lower, int64_t upper) const;

        public:

            /**
             * @brief This constructor loads a track file into its data storage while creating a new Track_data object.
             * The file needs to be a json file with the [format](@ref track_data_format), also specified in the [format converter](https://git.thm.de/bahn-simulator/format-converter).
             *
             * @param URI The location of the File
             */
            Track_data(const std::filesystem::path& URI);

            /**
             * @brief This constructor loads a track file into its data storage while creating a new Track_data object.
             * The file needs to be a json file with the [format](@ref track_data_format), also specified in the [format converter](https://git.thm.de/bahn-simulator/format-converter).
             *
             * @param URI The location of the File
             */
            Track_data(const std::string& URI);

            /**
             * @brief This constructor loads a track file into its data storage while creating a new Track_data object.
             * The file needs to be a json file with the [format](@ref track_data_format), also specified in the [format converter](https://git.thm.de/bahn-simulator/format-converter).
             *
             * @param URI The location of the File
             */
            Track_data(const char* URI);

            /**
             * @brief This constructor uses given json data to create a new Track_data object.
             * The data needs to be in the correct [format](@ref track_data_format).
             *
             * @param data
             */
            explicit Track_data(const json& data_json);

            /**
             * @brief Destroy the Track_data object
             *
             */
            ~Track_data();

            /**
             * @brief Get the Frame to the given location.
             * Because a binary search is used to find the frame and the location is a floating point value, the returned Frame might not be the nearest.
             *
             * @param location the location on the track in meters
             * @return int64_t the nearest frame to that location
             */
            int64_t getFrame(double location);

            /**
             * @brief Get the Frame to the given location.
             * Because a binary search is used to find the frame and the location is a floating point value, the returned Frame might not be the nearest.
             * This const version does not save the last returned index to speed the search up.
             *
             * @param location the location on the track in meters
             * @return int64_t the nearest frame to that location
             */
            int64_t getFrame(double location) const;

            /**
             * @brief Get the number of elements in the loaded json file
             *
             * @return int64_t the number of elements
             */
            int64_t getSize() const;

            /**
             * @brief checks if the everything was loaded correctly
             *
             * @return true the data is valid
             * @return false there was a problem loading the data
             */
            bool isValid() const;

            /**
             * @brief returns the last location in the dataset
             *
             * @return double the last location
             */
            double lastLocation() const;

            /**
             * @brief returns the first location in the dataset
             *
             * @return double the first location
             */
            double firstLocation() const;


        };
    }
}
