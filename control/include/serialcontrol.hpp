#pragma once

#include <chrono>
#include "common.hpp"
#include "core/include/input_axis.hpp"
#include "physics/include/physics.hpp"
#include <rs232.hpp>

namespace libtrainsim{
    class serial_channels{
        private :

                /**
            * @brief These variables save the individual channel-numbers.
            */
            std::string comport;
            sakurajin::Baudrate baudrate;
            int analog_drive,
            analog_brake,
            count_digital,
            drivemode_r,
            drivemode_0,
            drivemode_x,
            drivemode_v,
            sifa,
            n_max,
            digital_drive,
            digital_brake,
            emergency_brake,
            door_r,
            door_l,
            door_release;

            /**
            * @brief The filename of configuration.
            */
            std::string filename = "data/production_data/config_serial_input.json";

        public :

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

            /**
            * @brief This function filles the variables with the data of the config-file.
            */
            void read_config();

            /**
            * @brief This function returns the value of config, value specifies what the function gives back.
            */
            int get_config(std::string value);

            /**
            * @brief This function returns the baudrate.
            */
            sakurajin::Baudrate get_baud();

            /**
            * @brief This function returns the name of the COM-Port as string.
            */
            std::string get_cport();
     
    };

    class serialcontrol{
        private:

            /**
            * @brief The portnumber, the serial controller is connected to.
            */ 
            int cport_nr;

            /**
            * @brief flag, true if COMPort successfully opened.
            */
            int serialflag;

            /**
            * @brief flag, true if emergency-brake-button was pressed until train has stopped.
            */
            bool emergency_flag;

            /**
            * @brief object which handels the communication with the COM-Port.
            */
            std::unique_ptr<sakurajin::RS232> rs232_obj;

            /**
            * @brief object which handels the serial_channels class.
            */
            std::unique_ptr<serial_channels> serial_channels_obj;
            
        public:

            serialcontrol();

            /**
            * @brief The array containing all decoded data recieved by the serial input.
            */
            libtrainsim::serial_channels port[14];
    

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
            * @brief This function updates the serial status, reads and analyses new incoming telegrams.
            */
            void updateSerial();

            /**
            * @brief This function reads from a given filename given integers.
            */
            int get_json_data(std::string filename, std::string name);

            /**
            * @brief This function initializes the array of port[15] and fills it with starter elements.
            */
            void startup();

            /**
            * @brief This function returns the value of port[i].channel_value.
            */
            int get_serial(int i);

            /**
            * @brief This function sets the value for port[i].channel_value.
            */
            void set_serial(int i, int value);

            /**
            * @brief This function returns the value of serial_flag.
            */
            int get_serialflag();

            /**
            * @brief This function sets the value for serial_flag.
            */
            void set_serialflag(int value);

            /**
            * @brief This function returns the value of emergency_flag.
            */
            int get_emergencyflag();

            /**
            * @brief This function sets the value for emergency_flag.
            */
            void set_emergencyflag(int value);

            /**
            * @brief This function gets the speedlevel calculated as difference between acceleration and brake. 
            */
            libtrainsim::core::input_axis get_slvl();

            std::chrono::time_point<std::chrono::high_resolution_clock> now(){
                return std::chrono::high_resolution_clock::now();
            }
    };
}