#pragma once

#include <chrono>
#include "common.hpp"
#include "core/include/input_axis.hpp"
#include "physics/include/physics.hpp"

namespace libtrainsim{
    class serial_channels{
        private:

        public:

            /**
            * @brief This class contains all parameter the serial telegram sends.
            */

            /**
            * @brief The channel value 0/1 digital; 0-255 analog.
            */ 
            int channel_value;

            /**
            * @brief The effective speedlevel calculated as difference between acceleration and brake.
            */ 
            libtrainsim::core::input_axis effective_slvl; 
     
    };

    class serialcontrol{
        private:

            /**
            * @brief The portnumber, the serial controller is connected to.
            */ 
            int cport_nr;

            /**
            * @brief The filename of configuration.
            */
            std::string filename = "data/production_data/config_serial_input.json";

        public:

            /**
            * @brief public flag true if COMPort successfully opened.
            */
            bool serialflag;

            static bool emergency_flag;

            /**
            * @brief The array containing all decoded data recieved by the serial input.
            */
            static libtrainsim::serial_channels port[14];
    

            /**
            * @brief This function converts a given hexadecimal value into an integer.
            */
            int hex2int(char hex);

            /**
            * @brief This function returns the analog value which gets extracted out of a given telegram-array.
            */
            int get_value_analog (char v1, char v2);

            /**
            * @brief This function returns the digital value which gets extracted out of a given telegram-array.
            */
            int get_value_digital (char x);

            /**
            * @brief This function returns the portnumber which gets extracted out of a given telegram-array.
            */
            int get_portnumber(char int1, char int2);

            /**
            * @brief This funciton open COM-Port for serial input.
            */
            void openCOMPort();

            /**
            * @brief This function updates the serial status, reads and analyses new incoming telegrams.
            */
            void updateSerial();

            /**
            * @brief This function reads from a given filename given integers.
            */
            static int get_json_data(std::string filename, std::string name);

            /**
            * @brief This function initializes the array of port[15] and fills it with starter elements.
            */
            void startup();

            /**
            * @brief This function returns the value of port[i].channel_value.
            */
            static int get_serial(int i);

            /**
            * @brief This function sests the value for port[i].channel_value.
            */
            void set_serial(int i, int value);

            /**
            * @brief This function gets the speedlevel calculated as difference between acceleration and brake. 
            */
            static libtrainsim::core::input_axis get_slvl();

            static std::chrono::time_point<std::chrono::high_resolution_clock> now(){
                return std::chrono::high_resolution_clock::now();
            }
    };
}