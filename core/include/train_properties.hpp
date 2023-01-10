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

#include "unit_system.hpp"
#include "helper.hpp"

#include <filesystem>
#include <nlohmann/json.hpp>

namespace libtrainsim {
    namespace core {
        /**
         * @brief all of the types a train is allowed to have in the config
         */
        enum class trainType{
            //the train is a passenger train
            passenger,
            //the train is a cargo train
            cargo
        };
        
        /**
         * @brief This class loads all the relevant properties of a train from a json file and provides an interface to get them.
         * Look [here](@ref train_properties_format) for more details on the json format.
         *
         */
        class LIBTRAINSIM_EXPORT_MACRO train_properties {
        private:

            /**
             * @brief The name of the train model
             */
            std::string name;

            /**
             * @brief The mass of the train in kg.
             */
            sakurajin::unit_system::mass mass;

            /**
            * @brief The maximum Power of a train in W
            */
            sakurajin::unit_system::power maxPower;

            /**
             * @brief the surface area of the train
             */
            sakurajin::unit_system::area surfaceArea;
            
            /**
             * @brief the number of wagon the train has
             */
            unsigned int numberWagons;
            
            /**
             * @brief the length of each wagon
             */
            sakurajin::unit_system::length wagonLength;
            
            /**
             * @brief the length of the driving part of the train
             */
            sakurajin::unit_system::length driverLength;
            
            /**
             * The type of this train.
             * All of the valid types are defined in the trainType enum
             */
            trainType type = trainType::passenger;

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
            sakurajin::unit_system::mass getMass() const;

            /**
             * @brief The maximum of Power in W the Train can have.
             */
            sakurajin::unit_system::power getMaxPower() const;

            /**
             * @brief get the effective surface area of the train
             */
            sakurajin::unit_system::area getSurfaceArea() const;
            
            /**
             * @brief get the number of wagon the train has 8excluding the driver part
             */
            unsigned int getNumberWagons() const;

            /**
             *  @brief get the length of each wagon
             */
            sakurajin::unit_system::length getWagonLength() const;

            /**
             * @brief get the length of the driver part of the train
             */
            sakurajin::unit_system::length getDriverLength() const;
            
            /**
             * @brief get the type this train has
             */
            trainType getTrainType() const;

            /**
             * @brief This value is the cv value multiplied by the area the front of the train has (unit: N/Pa).
             * It is used to calulate the air drag force of the train, by multiplying it with the dynamic pressure using the current velocity.
             * This calculation is done by the calculateDrag function, which also adds the rolling resistance.
             * @deprecated will be removed in the near future. For now this will always return {} and thus is useless.
             */
            std::optional<double> getAirDrag() const;

            /**
             * @brief the rolling resistance coefficient between the train and the rails (no unit).
             * The default value is 0.2.
             * @deprecated will be removed in the near future. For now it always returns the default of 0.2
             */
            double getTrackDrag() const;

        };
    }
}
