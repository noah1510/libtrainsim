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
#include "helper.hpp"

#include <filesystem>
#include <nlohmann/json.hpp>

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
             * @brief The name of the train model
             */
            std::string name;

            /**
             * @brief The mass of the train in kg.
             */
            sakurajin::unit_system::base::mass mass;

            /**
            * @brief The maximum Power of a train in W
            */
            sakurajin::unit_system::common::power maxPower;


            /**
             * @brief This value is the cv value multiplied by the area the front of the train has (unit: N/Pa).
             * It is used to calulate the air drag force of the train, by multiplying it with the dynamic pressure using the current velocity.
             * This calculation is done by the calculateDrag function, which also adds the rolling resistance.
             */

            std::optional<double> air_drag = 0.0;

            /**
             * @brief the rolling resistance coefficient between the train and the rails (no unit).
             * The default value is 0.02.
             */
            double track_drag = 0.02;

            /**
             * @brief Loads the data_json into the other menbers;
             */
            void loadJsonData(const nlohmann::json& data_json);

        public:
            /**
             * @brief Construct a new train properties object from a given json file.
             * @note The data needs the correct [format](@ref train_properties_format).
             * @param URI The location of the json file.
             */
            train_properties(const std::filesystem::path& URI);

            /**
             * @brief Construct a new train properties object from json data
             * @note The data needs the correct [format](@ref train_properties_format).
             *
             * @param data The json data.
             */
            explicit train_properties(const nlohmann::json& data);

            /**
             * @brief The name of the train model
             */
            const std::string& getName() const;

            /**
             * @brief The mass of the train in kg.
             */
            sakurajin::unit_system::base::mass getMass() const;

            /**
             * @brief The maximum of Power in W the Train can have.
             */

            sakurajin::unit_system::common::power getMaxPower() const;


            /**
             * @brief This value is the cv value multiplied by the area the front of the train has (unit: N/Pa).
             * It is used to calulate the air drag force of the train, by multiplying it with the dynamic pressure using the current velocity.
             * This calculation is done by the calculateDrag function, which also adds the rolling resistance.
             */
            std::optional<double> getAirDrag() const;

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
            sakurajin::unit_system::common::force calulateDrag(sakurajin::unit_system::common::speed currentVelocity) const;

            /**
             * @brief clamps the given velocity to the max velocity
             *
             * @param currentVelocity the verlocity that should be clamped
             * @return double the clamped velocity
             */
            sakurajin::unit_system::common::speed clampVelocity(sakurajin::unit_system::common::speed currentVelocity) const;

            /**
             * @brief clamps the given accelleration to the max velocity
             *
             * @param currentVelocity the accelleration that should be clamped
             * @return double the clamped accelleration
             */
            sakurajin::unit_system::common::acceleration clampAcceleration(sakurajin::unit_system::common::acceleration currentAcceleration) const;

        };
    }
}
