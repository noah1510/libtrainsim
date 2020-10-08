#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace libtrainsim {
    /**
     * @brief This class loads all the relevant properties of a train from a json file and provides an interface to get them.
     * 
     */
    class train_properties{
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
            double air_drag;

            /**
             * @brief the rolling resistance coefficient between the train and the rails (no unit).
             * The default value is 0.002.
             */
            double track_drag = 0.002;

        public:
            /**
             * @brief Construct a new train properties object from a given json file.
             * @param URI The location of the json file.
             */
            train_properties(const std::filesystem::path& URI);

            /**
             * @brief checks if the object contains valid data.
             * 
             * @return true the object is loaded with valid data.
             * @return false there was an error while loading the data.
             */
            bool isValid() const;

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