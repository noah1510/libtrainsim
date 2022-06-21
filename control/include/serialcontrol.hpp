#pragma once

#include <chrono>
#include "common.hpp"
#include "core/include/input_axis.hpp"
#include <rs232.hpp>
#include <memory>
#include <vector>

namespace libtrainsim{
    /**
    * @brief This class contains all parameter the serial telegram sends.
    */
    class serial_channel{
        private :

        public :

            serial_channel(std::string n, int ch, std::string t);

            /**
            * @brief channel-function.
            */
            std::string name;

            /**
            * @brief channel-number.
            */
            int channel;

            /**
            * @brief channel-type (analog/digital).
            */
            std::string type;

            /**
            * @brief The channel value 0/1 digital; 0-255 analog.
            */
            int value;

            /**
            * @brief This function returns the value of config, value specifies what the function gives back.
            */
            int get_config(std::string value);     
    };

    /**
    * @brief This class contains all variables and functions to handle hardware input.
    */
    class serialcontrol{
        private:

            /**
            * @brief comport given by config-file.
            */
            std::string comport;

            /**
            * @brief baudrate given by config-file.
            */
            sakurajin::Baudrate baudrate;

            /**
            * @brief flag, true if COMPort successfully opened.
            */
            bool isConnected;

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
            std::vector<serial_channel> serial_channels;

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
            * @brief This function sets the value for searched channelnumber i.
            */
            void set_serial(int i, int value, bool isAnalog);
            
        public:

            serialcontrol(std::string filename);

            /**
            * @brief This function filles the variables with the data of the config-file.
            */
            void read_config(std::string filename);

            /**
            * @brief This function returns the baudrate.
            */
            sakurajin::Baudrate get_baud();

            /**
            * @brief This function returns the name of the COM-Port as string.
            */
            std::string get_cport();

            /**
            * @brief This function updates the serial status, reads and analyses new incoming telegrams.
            */
            void update();

            /**
            * @brief This function returns the value of searched channel.
            */
            int get_serial(std::string name);

            /**
            * @brief This function returns the value of isConnected.
            */
            bool get_isConnected();

            /**
            * @brief This function sets the value for isConnected.
            */
            void set_isConnected(bool value);

            /**
            * @brief This function returns the value of emergency_flag.
            */
            int get_emergencyflag();

            /**
            * @brief This function gets the speedlevel calculated as difference between acceleration and brake. 
            */
            libtrainsim::core::input_axis get_slvl();
    };
}