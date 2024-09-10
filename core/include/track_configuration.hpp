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

#include "train_properties.hpp"

namespace libtrainsim::core {
    /**
     * @brief This class stores the data of a single data point from the track data.
     * For the format of the json files look [here](@ref track_data_format).
     */
    class LIBTRAINSIM_EXPORT_MACRO Track_data_point {
      public:
        /**
         * @brief The frame number of the data point
         */
        const uint64_t Frame;

        /**
         * @brief The location along the track in m.
         */
        const sakurajin::unit_system::length Location;

        /**
         * @brief This is the current Radius of the Track
         */
        const double Radius;

        /**
         * @brief This is the current slope of the Track in degrees
         */
        const double Slope;

        /**
         * @brief The multiplier for the Track friction for the current position
         */
        const std::optional<double> FrictionMultiplier;

        /**
         * @brief Create a data point with all values.
         * the optional values are optional (huge surprise i know) and may be {}.
         */
        Track_data_point(uint64_t                       _frame,
                         sakurajin::unit_system::length _location,
                         double                         _radius,
                         double                         _slope,
                         std::optional<double>          _frictionMultiplier);
    };

    // an underground data point as defined in the [track json documentation](@ref track_format)
    class LIBTRAINSIM_EXPORT_MACRO undergroundDataPoint
        : public std::tuple<sakurajin::unit_system::length, sakurajin::unit_system::length, sakurajin::unit_system::area> {
      public:
        /**
         * @brief construct an underground data point
         */
        undergroundDataPoint(sakurajin::unit_system::length _begin,
                             sakurajin::unit_system::length _end,
                             sakurajin::unit_system::area   _area);

        /**
         * @brief the point where the line begins
         */
        [[nodiscard]]
        const sakurajin::unit_system::length& begin() const;

        /**
         * @brief the point where the line ends
         */
        [[nodiscard]]
        const sakurajin::unit_system::length& end() const;

        /**
         * @brief the point where the line ends
         */
        [[nodiscard]]
        const sakurajin::unit_system::area& area() const;
    };

    // All the valid stop types which are defined in the [json format documentation](@ref stops_data_format).
    enum stopTypes { station = 0 };

    // A class to unpack a stop object specified in the [json format documentation](@ref stops_data_format).
    class LIBTRAINSIM_EXPORT_MACRO stopDataPoint : public std::tuple<std::string, sakurajin::unit_system::length, stopTypes> {
      public:
        /**
         * @brief construct a new stop data point
         */
        stopDataPoint(const std::string& _name, sakurajin::unit_system::length _position, stopTypes _type);

        // get the name of the stop
        [[nodiscard]]
        const std::string& name() const;

        // get the position of the stop
        [[nodiscard]]
        const sakurajin::unit_system::length& position() const;

        // get the type of this stop
        [[nodiscard]]
        const stopTypes& type() const;
    };

    /**
     * @brief This class is used to load a track json file containing the definition of the [track](@ref track_format).
     */
    class LIBTRAINSIM_EXPORT_MACRO Track {
      private:
        /**
         * @brief The train data of the track.
         * This specifies all relevant information about the train used on this track.
         *
         */
        std::optional<train_properties> train_dat;

        // The fallback friction multiplier in case the Track data has not defined a value for that point.
        double defaultTrackFrictionMultiplier = 1.0;

        // The vector containing all the underground data points
        std::vector<undergroundDataPoint> undergroundData;

        // a vector containing all the stops
        std::vector<stopDataPoint> stopsData;

        // a vector containing all the stations. This will be initialized when needed.
        std::vector<stopDataPoint> stationsData;

        /**
         * @brief The location where the train should start in the beginning.
         *
         */
        sakurajin::unit_system::length startingPoint;

        /**
         * @brief The location where the train should end.
         *
         */
        sakurajin::unit_system::length endPoint;

        /**
         * @brief The name of the track
         *
         */
        std::string name;

        /**
         * @brief The location of the video file for this track
         *
         */
        std::filesystem::path videoFile;

        // the parent Path to the json data
        std::filesystem::path parentPath;

        // if it has a value it is the (partly) unparsed json data
        std::optional<nlohmann::json> data_json;

        // load the track data in case it was not loaded yet
        void parseTrack();

        /**
         * @brief The parsed data of the current Track.
         *
         */
        std::vector<Track_data_point> data;

        /**
         * @brief this is the binary search to find the frame of a given location, between the lower and upper bound with the a given
         * starting index. This algorithm might not return what is actually the nearest frame to the wanted value. Since a binary search
         * is performed the second closest value might be the one that will be returned.
         *
         * @param location the location on the track in meters
         * @param index the first index to be checked
         * @param lower the lower search bound
         * @param upper the upper serch bound
         * @return int64_t the nearest frame to that location
         */
        [[nodiscard]]
        uint64_t getFrame_c(sakurajin::unit_system::length location) const;

        /**
         * @brief parse the given json data into all of the class variables
         */
        void parseJsonData();

      public:
        // this gets rid of a default constructor
        Track() = delete;

        /**
         * @brief Create a track from a given json file.
         * @note The json file need the correct [format](@ref track_format).
         *
         * @param URI The location of the File
         * @param lazyLoad true if you only want to load the data on the first
         */
        explicit Track(const std::filesystem::path& URI, bool lazyLoad = false);

        /**
         * @brief Create a track from json data and a parent path.
         * @note The json file need the correct [format](@ref track_format).
         *
         * @param data_json The json data
         * @param parentPath The path this config is loaded from (needed to locate the video)
         * @param lazyLoad true if you only want to load the data on the first
         */
        Track(const nlohmann::json& data_json, const std::filesystem::path& parentPath, bool lazyLoad = false);

        /**
         * @brief this function ensures that the data is fully loaded.
         */
        void ensure();

        /**
         * @brief Get the Frame to the given location.
         * Because a binary search is used to find the frame and the location is a floating point value, the returned Frame might not be
         * the nearest. This const version does not save the last returned index to speed the search up.
         *
         * @param location the location on the track in meters
         * @return int64_t the nearest frame to that location
         */
        [[nodiscard]]
        uint64_t getFrame(sakurajin::unit_system::length location) const;

        /**
         * @brief Get the data point to the given location.
         * Because a binary search is used to find the frame and the location is a floating point value, the returned Frame might not be
         * the nearest. This const version does not save the last returned index to speed the search up.
         *
         * @param location the location on the track in meters
         * @return const Track_data_point& the nearest data point to that location
         */
        [[nodiscard]]
        const Track_data_point& getDataPointAt(sakurajin::unit_system::length location) const;

        /**
         * @brief returning the train data of this track
         *
         * @return const train_properties& the train data of this track
         */
        [[nodiscard]]
        const train_properties& train() const;

        /**
         * @brief The multiplier for the Track friction for the current position
         */
        [[nodiscard]]
        double get_frictionMultiplier(sakurajin::unit_system::length location) const;

        /**
         * @brief returns the last location of the track.
         *
         * @return double the last location
         */
        [[nodiscard]]
        sakurajin::unit_system::length lastLocation() const;

        /**
         * @brief returns the first location of the track.
         *
         * @return double the first location
         */
        [[nodiscard]]
        sakurajin::unit_system::length firstLocation() const;

        /**
         * @brief Get the path to the video file.
         *
         * @return std::filesystem::path the path to the video file
         */
        [[nodiscard]]
        std::filesystem::path getVideoFilePath() const;

        // get the name of the track
        [[nodiscard]]
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
        [[nodiscard]]
        std::tuple<bool, sakurajin::unit_system::area, sakurajin::unit_system::length>
        getUndergroundInfo(sakurajin::unit_system::length position) const;

        /**
         * @brief returns all of the stops this track has defined
         *
         * This will always have at least two values to indicate the begin and end
         * of the Track. If the underlying Track_data is not fully loaded this will
         * throw an error. To prevent this call enusure before calling this function.
         */
        [[nodiscard]]
        const std::vector<stopDataPoint>& getStations() const;

        /**
         * @brief set where this track should end
         *
         * @note this function is not needed for operation but can be used to
         * specify at which stop the simulator should close
         */
        void setLastLocation(sakurajin::unit_system::length);

        /**
         * @brief set where this track should begin
         *
         * @note this function is not needed for operation but can be used to
         * specify at which stop the simulator should open
         */
        void setFirstLocation(sakurajin::unit_system::length);
    };
} // namespace libtrainsim::core
