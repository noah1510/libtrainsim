#pragma once

#include <chrono>
#include "common.hpp"
#include "core/include/input_axis.hpp"
#include "helper.hpp"
#include <rs232.hpp>
#include <memory>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <future>

namespace libtrainsim{
    namespace control{
        /**
        * @brief This class contains all parameter the serial telegram sends.
        */
        class serial_channel{
            private :

            public :

                /**
                * @brief constructor asigns given variables to class-members.
                */
                serial_channel(const std::string& n, int ch, const std::string& t, const std::string& dir);

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
                 * @brief the direction of the data transfer (input or output)
                 */
                std::string direction;
        };

        /**
        * @brief This class contains all variables and functions to handle hardware input.
        */
        class serialcontrol{
            private:
                
                /**
                 * @brief a single mutex to secure the data access
                 */
                std::shared_mutex accessMutex;

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
                bool isConnected = false;
                
                /**
                 * @brief a separate mutex for the isConnected variable
                 */
                std::shared_mutex connectedMutex;

                /**
                * @brief flag, true if emergency-brake-button was pressed until train has stopped.
                */
                bool emergency_flag = false;

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
                int hex2int(char hex) const;
                
                /**
                 * @brief returns the decoded telegram:
                 * @return std::tuple<uint8_t, uint8_t, bool, int> {port, value, isDigital, error code}
                */
                std::tuple<uint8_t, uint8_t, bool, int> decodeTelegram(const std::string& telegram) const;

                /**
                * @brief This function sets the value for searched channelnumber i.
                */
                void set_serial(int i, int value, bool isAnalog);

                /**
                * @brief This function filles the variables with the data of the config-file.
                */
                void read_config(const std::filesystem::path& filename);
                
                /**
                 * @brief this stores the job to update the serial interface on a different thread
                 */
                std::future<void> updateLoop;
                
            public:

                /**
                * @brief constructor creates an object with all needed parts to handle hardware input.
                * @note as soon as it is contructed it automatically updates the values from a second thread until the object should be destroyed.
                */
                serialcontrol(const std::filesystem::path& filename);
                
                /**
                 * @brief destroy the serial control object
                 */
                ~serialcontrol();

                /**
                * @brief This function returns the value of searched channel by name / funciton. Return value is the current value of searched channel. Can be used to get the last known value of different functions the hardware input has.
                */
                int get_serial(std::string name);

                /**
                * @brief This function returns the value of isConnected.
                */
                bool IsConnected();

                /**
                * @brief This function returns the value of emergency_flag.
                */
                bool get_emergencyflag();

                /**
                * @brief This function gets the speedlevel calculated as difference between acceleration and brake. 
                */
                libtrainsim::core::input_axis get_slvl();
        };
    }
}
