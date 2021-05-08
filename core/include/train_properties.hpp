/**
 * @file train_properties.hpp
 * @author Noah Kirschmann (noah.kirschmann@mnd.thm.de)
 * @brief This file contains the definition of the train_properties to manage train models.
 * @version 0.4.0
 * @date 2020-10-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include "types.hpp"

#include "common.hpp"

#include <filesystem>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace libtrainsim {
    namespace core {
        /**
         * @brief This class loads all the relevant properties of a train from a json file and provides an interface to get them.
         * Look [here](@ref train_properties_format) for more details on the json format.
         *
         */
        class train_properties {
        private:

            /**
             * @brief The loaded json data, read the docs for more details on the format of the train_properties.
             */
            json data_json;

            /**
             * @brief True if an error has happened causing the props to be invalid.
             */
            bool hasError = true;

            /**
             * @brief The name of the train model
             */
            std::string name;

            /**
             * @brief The maximum velocity the train can have in m/s.
             */
            double max_velocity;

            /**
             * @brief The maximum acceleration the train can have in m/(s^2).
             */
            double max_acceleration;

            /**
             * @brief The mass of the train in kg.
             */
            double mass;

            /**
             * @brief This value is the cv value multiplied by the area the front of the train has (unit: N/Pa).
             * It is used to calulate the air drag force of the train, by multiplying it with the dynamic pressure using the current velocity.
             * This calculation is done by the calculateDrag function, which also adds the rolling resistance.
             */
            double air_drag = 0.0;

            /**
             * @brief the rolling resistance coefficient between the train and the rails (no unit).
             * The default value is 0.002.
             */
            double track_drag = 0.002;

            /**
             * @brief Loads the data_json into the other menbers;
             */
            void loadJsonData();

        public:
            /**
             * @brief Construct a new train properties object from a given json file.
             * @note The data needs the correct [format](@ref train_properties_format).
             * @param URI The location of the json file.
             */
            train_properties(const std::filesystem::path& URI);

            /**
             * @brief Construct a new train properties object from a given json file.
             * @note The data needs the correct [format](@ref train_properties_format).
             * @param URI The location of the json file.
             */
            train_properties(const std::string& URI);

            /**
             * @brief Construct a new train properties object from a given json file.
             * @note The data needs the correct [format](@ref train_properties_format).
             * @param URI The location of the json file.
             */
            train_properties(const char* URI);

            /**
             * @brief Construct a new train properties object from json data
             * @note The data needs the correct [format](@ref train_properties_format).
             *
             * @param data The json data.
             */
            explicit train_properties(const json& data);

            /**
             * @brief checks if the object contains valid data.
             *
             * @return true the object is loaded with valid data.
             * @return false there was an error while loading the data.
             */
            bool isValid() const;

            /**
             * @brief The name of the train model
             */
            const std::string& getName() const;

            /**
             * @brief The maximum velocity the train can have in m/s.
             */
            double getMaxVelocity() const;

            /**
             * @brief The maximum acceleration the train can have in m/(s^2).
             */
            double getMaxAcceleration() const;

            /**
             * @brief The mass of the train in kg.
             */
            double getMass() const;

            /**
             * @brief This value is the cv value multiplied by the area the front of the train has (unit: N/Pa).
             * It is used to calulate the air drag force of the train, by multiplying it with the dynamic pressure using the current velocity.
             * This calculation is done by the calculateDrag function, which also adds the rolling resistance.
             */
            double getAirDrag() const;

            /**
             * @brief the rolling resistance coefficient between the train and the rails (no unit).
             * The default value is 0.002.
             */
            double getTrackDrag() const;

            /**
             * @brief This function calculates the drag force based on the current velocity.
             *
             * @param currentVelocity the current velocity in m/s
             * @return double the drag force in N
             */
            double calulateDrag(double currentVelocity) const;

            /**
             * @brief clamps the given velocity to the max velocity
             *
             * @param currentVelocity the verlocity that should be clamped
             * @return double the clamped velocity
             */
            double clampVelocity(double currentVelocity) const;

            /**
             * @brief clamps the given accelleration to the max velocity
             *
             * @param currentVelocity the accelleration that should be clamped
             * @return double the clamped accelleration
             */
            double clampAcceleration(double currentAcceleration) const;

        };
    }
}
