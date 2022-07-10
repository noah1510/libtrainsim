#pragma once
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

#include "track_data.hpp"
#include "train_properties.hpp"

#include "length.hpp"
#include "helper.hpp"

#include <filesystem>
#include <optional>
#include <nlohmann/json.hpp>

namespace libtrainsim{
    namespace core {

        /**
         * @brief This class is used to load a track json file containing the definition of the [track](@ref track_format).
         */
        class Track {
        private:

            /**
             * @brief The data points of the track.
             * This maps locations to frames.
             *
             */
            std::optional<Track_data> track_dat;

            /**
             * @brief The train data of the track.
             * This specifies all relevant information about the train used on this track.
             *
             */
            std::optional<train_properties> train_dat;

            /**
             * @brief The location where the train should start in the beginning.
             *
             */
            sakurajin::unit_system::base::length startingPoint;

            /**
             * @brief The location where the train should end.
             *
             */
            sakurajin::unit_system::base::length endPoint;

            /**
             * @brief The name of the track
             *
             */
            std::string name;
            
            /**
             * @brief parse the given json data into all of the class variables
             */
            void parseJsonData(const nlohmann::json& data_json, const std::filesystem::path& p);

            /**
             * @brief The location of the video file for this track
             *
             */
            std::filesystem::path videoFile;

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
             * @brief returns the last location of the track.
             *
             * @return double the last location
             */
            sakurajin::unit_system::base::length lastLocation() const;

            /**
             * @brief returns the first location of the track.
             *
             * @return double the first location
             */
            sakurajin::unit_system::base::length firstLocation() const;

            /**
             * @brief Get the path to the video file.
             *
             * @return std::filesystem::path the path to the video file
             */
            std::filesystem::path getVideoFilePath() const;

        };
    }
}
