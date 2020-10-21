/**
 * @file track_configuration.hpp
 * @author Noah Kirschmann (noah.kirschmann@mnd.thm.de)
 * @brief This file contains the definition of the Track class to manage tracks.
 * @version 0.4.0
 * @date 2020-10-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include "track_data.hpp"
#include "train_properties.hpp"

#include <filesystem>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace libtrainsim::core{

    /**
     * @brief This class is used to load a track json file containing the definition of the [track](@ref track_format).
     * @todo implement the class.
     * @warning Nothing here soes what it should at the moment so so not use this class.
     */
    class Track{
        private:

            /**
             * @brief This object contains the json data of the current track.
             * 
             */
            json data_json;
            
            /**
             * @brief The data points of the track.
             * This maps locations to frames.
             * 
             */
            Track_data track_dat = Track_data(std::filesystem::path(""));
            
            /**
             * @brief The train data of the track.
             * This specifies all relevant information about the train used on this track.
             * 
             */
            train_properties train_dat = train_properties(std::filesystem::path(""));
            
            /**
             * @brief The location where the train should start in the beginning.
             * 
             */
            double startingPoint;
            
            /**
             * @brief The location where the train should end.
             * 
             */
            double endPoint;
            
            /**
             * @brief The name of the track
             * 
             */
            std::string name;
            
            /**
             * @brief true if an error has happened. 
             */
            bool hasError = true;
            
            Track() = delete;

        public:
            
            /**
             * @brief Create a track from a given json file.
             * @note The json file need the correct [format](@ref track_format).
             * 
             * @param URI The location of the File
             */
            Track(const std::filesystem::path& URI);
            
            /**
             * @brief returning the Track_data of this track
             * 
             * @return const Track_data& the track data
             */
            const Track_data& data() const;
            
            /**
             * @brief returning the train data of this track
             * 
             * @return const train_properties& the train data of this track
             */
            const train_properties& train() const;
            
            /**
             * @brief returns if the track was loaded correctly
             * 
             * @return true everything was loaded correctly
             * @return false there was an error while loading the track or its data
             */
            bool isValid() const;
            
    };
}
