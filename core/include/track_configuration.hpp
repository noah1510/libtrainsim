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
#include "area.hpp"
#include "helper.hpp"

#include <filesystem>
#include <optional>
#include <cmath>
#include <tuple>
#include <nlohmann/json.hpp>

namespace libtrainsim{
    namespace core {
        //an underground data point as defined in the [track json documentation](@ref track_format)
        class undergorundDataPoint : public std::tuple<
            sakurajin::unit_system::base::length,
            sakurajin::unit_system::base::length,
            sakurajin::unit_system::common::area
        >{
        public:
            /**
             * @brief construct an underground data point
             */
            undergorundDataPoint(
                sakurajin::unit_system::base::length _begin,
                sakurajin::unit_system::base::length _end,
                sakurajin::unit_system::common::area _area
            );
            
            /**
             * @brief the point where the line begins
             */
            const sakurajin::unit_system::base::length& begin() const;
            
            /**
             * @brief the point where the line ends
             */
            const sakurajin::unit_system::base::length& end() const;
            
            /**
             * @brief the point where the line ends
             */
            const sakurajin::unit_system::common::area& area() const;
        };

        //All of the valid stop types which are defined in the [json format documentation](@ref stops_data_format).
        enum stopTypes{
            station = 0
        };
        
        //A class to unpack a stop object specified in the [json format documentation](@ref stops_data_format).
        class stopDataPoint: public std::tuple<
            std::string,
            sakurajin::unit_system::base::length,
            stopTypes
        >{
        public:
            /**
             * @brief construct a new stop data point
             */
            stopDataPoint(
                std::string _name,
                sakurajin::unit_system::base::length _position,
                stopTypes _type
            );
            
            //get the name of the stop
            const std::string& name() const;
            
            //get the position of the stop
            const sakurajin::unit_system::base::length& position() const;
            
            //get the type of this stop
            const stopTypes& type() const;
            
        };
        
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
            
            //The fallback friction multiplier in case the Track data has not defined a value for that point.
            double defaultTrackFrictionMultiplier = 1.0;
            
            //The vector containing all of the underground data points
            std::vector<undergorundDataPoint> undergroundData;
            
            //a vector containing all of the stops
            std::vector<stopDataPoint> stopsData;

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
            void parseJsonData();

            /**
             * @brief The location of the video file for this track
             *
             */
            std::filesystem::path videoFile;

            //this gets rid of a defauolt constructor
            Track() = delete;
            
            //the parent Path to the json data
            std::filesystem::path parentPath;
            
            //if it has a value it is the (partly) unparsed json data
            std::optional<nlohmann::json> data_json;
            
            //load the track data in case it was not loaded yet
            void parseTrack();

        public:

            /**
             * @brief Create a track from a given json file.
             * @note The json file need the correct [format](@ref track_format).
             *
             * @param URI The location of the File
             * @param lazyLoad true if you only want to load the data on the first
             */
            Track(const std::filesystem::path& URI, bool lazyLoad = false);

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
            
            //get the name of the track
            const std::string& getName() const;
            
            /**
             * @brief get the underground data for a given position
             * 
             * By default it is assumed that the train is not underground.
             * This function returns three values in a tuple:
             * 
             *  * bool -> if that position is underground this is true
             *  * area -> the area of the tunnel
             *  * length -> the remaining length of the tunnel
             */
            std::tuple<
                bool, 
                sakurajin::unit_system::common::area, 
                sakurajin::unit_system::base::length
            > getUndergroundInfo(sakurajin::unit_system::base::length position) const;
            
            /**
             * @brief returns all of the stops this track has defined 
             * 
             * This will always have at least two values to indicate the begin and end
             * of the Track. If the underlying Track_data is not fully loaded this will
             * throw an error. To prevent this call enusure before calling this function.
             */
            const std::vector<stopDataPoint>& getStops() const;
            
            /**
             * @brief this function ensures that the data is fully loaded.
             */
            void ensure();

        };
    }
}
