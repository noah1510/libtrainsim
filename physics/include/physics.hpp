#pragma once

#include "core.hpp"

namespace libtrainsim {

    class LIBTRAINSIM_EXPORT_MACRO [[maybe_unused]] physics {
      private:
        std::shared_mutex mutex_data;

        /**
         * @brief The current acceleration of the train in ms^-2.
         */
        sakurajin::unit_system::acceleration current_acceleration;

        /**
         * @brief The current velocity of the train in ms^-1.
         */
        sakurajin::unit_system::speed velocity;

        /**
         * @brief The highest possible velocity of train.The value can be changed to any other value which makes sense.
         * In the future this can be calculated by the resistance forces of Track and Train as well as the maximum of Trainpower.
         */
        const sakurajin::unit_system::speed MaxVelocity{85.0};

        /**
         * @brief The current location of the train at the track.
         */
        sakurajin::unit_system::length location;

        /**
         * @brief The current Traction of the Train
         */
        sakurajin::unit_system::force currTraction;

        /**
         * @brief The current Power of the Train. This variable is calculated and therefore not a const variable
         */
        sakurajin::unit_system::power currPower;

        /**
         * @brief The current Speedlevel of the Train. This variable is set by playerinput, the default value is 0.0
         */
        long double speedlevel = 0.0;

        const libtrainsim::core::Track config;

        std::chrono::time_point<std::chrono::high_resolution_clock> last_update;

        std::shared_mutex mutex_error;
        bool              hasError = true;

        const bool autoTick;

        /**
         * @brief This function calculates the maximum amount of the Force the Train can apply. This amount depends on the mass of the Train
         * and the resistance force between train and track.
         */
        [[nodiscard]]
        sakurajin::unit_system::force
        calcMaxForce(sakurajin::unit_system::mass mass, sakurajin::unit_system::acceleration g, long double train_drag) const;

        /**
         * @brief This function calculates the whole amount of the resistance forces and adds them together. Therefore this function uses
         * the different types of drag coefficients from the traindata and the trackdata, Furthermore it uses the mass of the train as well
         * as the location to get the train_drag coefficient correctly. This function is not implemented yet and it will return 0 by using
         * it.
         */
        [[maybe_unused]] [[nodiscard]]
        sakurajin::unit_system::force calcDrag();

        // true if the emergency break is activated.
        // the train has to break full until it is stopped, after that it is allowed to accellerate again.
        bool isEmergencyBreaking = false;

        /**
         * @brief Calls the tick function if autoTick is enabled.
         */
        void doAutoTick();

      public:
        [[maybe_unused]] explicit physics(const libtrainsim::core::Track& conf, bool _autoTick = false);

        ~physics();

        [[maybe_unused]] [[nodiscard]]
        bool isValid();

        /**
         * @brief This function returns the current speedlevel of the train, which is set through the actions of a player.
         * If autoTick is true, this function will automatically call tick before returning the value.
         */
        [[maybe_unused]]
        void setSpeedlevel(const core::input_axis& slvl);

        /**
         * @brief This function returns the current velocity of the train. If autoTick is true, this function will automatically call tick
         * before returning the value.
         */
        [[maybe_unused]] [[nodiscard]]
        sakurajin::unit_system::speed getVelocity();

        /**
         * @brief This function returns the current location of the train along the track.
         * If autoTick is true, this function will automatically call tick before returning the value.
         */
        [[maybe_unused]] [[nodiscard]]
        sakurajin::unit_system::length getLocation();

        /**
         * @brief This function returns the current acceleration of the train. If autoTick is true, this function will automatically call
         * tick before returning the value.
         */
        [[maybe_unused]] [[nodiscard]]
        sakurajin::unit_system::acceleration getAcceleration();

        /**
         * @brief This function returns the current traction of the train. If autoTick is true, this function will automatically call tick
         * before returning the value.
         */
        [[maybe_unused]] [[nodiscard]]
        sakurajin::unit_system::force getTraction();

        /**
         * @brief This function returns the current power of the train. If autoTick is true, this function will automatically call tick
         * before returning the value.
         */
        [[maybe_unused]] [[nodiscard]]
        sakurajin::unit_system::power getCurrPower();

        /**
         * @brief activate the emergency break
         *
         * This causes the train to break until it comes to a stop.
         */
        [[maybe_unused]]
        void emergencyBreak();

        [[maybe_unused]] [[nodiscard]]
        bool reachedEnd();

        /**
         * @brief This Function does the whole mathematics behind the physics. It calculates the current values of location, velocity,
         * acceleration, traction and trainpower for every tick the simulation is running.
         */
        void tick();
    };
} // namespace libtrainsim
