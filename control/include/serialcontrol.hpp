#pragma once

#include "helper.hpp"
#include "input_axis.hpp"
#include "unit_system.hpp"
#include "eventSystem.hpp"

#include <nlohmann/json.hpp>
#include <rs232.hpp>

#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>

namespace libtrainsim{
    namespace control{
        /**
        * @brief This class contains all parameter the serial telegram sends.
        */
        class LIBTRAINSIM_EXPORT_MACRO serial_channel{
            public :

                /**
                * @brief constructor assigns given variables to class-members.
                */
                serial_channel(const std::string& n, int ch, bool tAnalog, bool dirIn);
                serial_channel() = default;

                /**
                * @brief channel-function.
                */
                std::string name = "empty";

                /**
                * @brief channel-number.
                */
                int channel = 0;

                /**
                * @brief channel-type (analog/digital).
                * 0 = digital
                * 1 = analog
                */
                bool isAnalog = false;

                /**
                * @brief The channel value 0/1 digital; 0-255 analog.
                */
                int value = 0;
                
                /**
                 * @brief the direction of the data transfer (input or output)
                 * true = input
                 * false = output
                 */
                bool directionInput = true;
        };

        /**
        * @brief This class contains all variables and functions to handle hardware input.
        */
        class LIBTRAINSIM_EXPORT_MACRO serialcontrol : public SimpleGFX::eventPoller{
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
                 * @return serial_channel with the decoded data
                */
                serial_channel decodeTelegram(const std::string& telegram) const;

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
                * @brief This function returns the value of isConnected.
                */
                bool IsConnected();
        };
    }
}
