#pragma once

#ifndef LIBTRAINSIM_EXPORT_MACRO
    #define LIBTRAINSIM_EXPORT_MACRO
#endif

namespace libtrainsim {
    namespace control {
        /**
         * @brief This class contains all parameter the serial telegram sends.
         */
        class LIBTRAINSIM_EXPORT_MACRO serial_channel {
          public:
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
        class LIBTRAINSIM_EXPORT_MACRO serialcontrol : public SimpleGFX::tracked_eventPoller {
          private:
            /**
             * @brief object which handels the communication with the COM-Port.
             */
            std::unique_ptr<sakurajin::RS232> rs232_obj;

            /**
             * @brief object which handels the serial_channels class.
             */
            std::vector<serial_channel> serial_channels;

            /**
             * @brief The baudrate to use for the serial connection
             * @warning it is assumed to be constant outside of the constructor
             */
            sakurajin::Baudrate baudrate;

            /**
             * @brief a single mutex to secure the data access
             */
            std::shared_mutex channelMutex;

            /**
             * @brief this stores a pointer to the simulator configuration
             */
            std::shared_ptr<libtrainsim::core::simulatorConfiguration> config;

            /**
             * @brief The regex to check if a string is a telegram
             */
            const std::regex telegramRegex{"X[UV][0-9]{2}[0]{2}[0-9A-F]{2}Y"};

            /**
             * @brief This function converts a given hexadecimal value into an integer.
             */
            [[nodiscard]]
            static inline int64_t hex2int(std::string hex) {

                int64_t val = 0;
                int64_t position_value = 1;
                uint64_t index = 0;

                std::reverse(hex.begin(), hex.end());

                for (auto c : hex) {
                    int digitVal = 0;
                    if (c >= '0' && c <= '9') {
                        digitVal = c - '0';
                    }else if (c >= 'A' && c <= 'F') {
                        digitVal = c - 'A' + 10;
                    }else{
                        return -1;
                    }

                    val += digitVal * position_value;

                    //bitshift by 4 equals multiplication by 16
                    position_value = position_value << 4;
                    index++;
                }

                return val;
            }

            /**
             * @brief returns the decoded telegram:
             * @return serial_channel with the decoded data
             */
            [[nodiscard]]
            serial_channel decodeTelegram(const std::string& telegram);

            /**
             * @brief This function filles the variables with the data of the config-file.
             */
            sakurajin::Baudrate read_config(const std::filesystem::path& filename);

          public:
            /**
             * @brief constructor creates an object with all needed parts to handle hardware input.
             * @note as soon as it is contructed it automatically updates the values from a second thread until the object should be
             * destroyed.
             */
            explicit serialcontrol(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _config);

            /**
             * @brief destroy the serial control object
             */
            ~serialcontrol() override;

            void operator()(SimpleGFX::eventManager& manager) override;

            void disconnect();

            bool connect();

            /**
             * @brief This function returns the value of isConnected.
             */
            [[nodiscard]]
            bool IsConnected();
        };
    } // namespace control
} // namespace libtrainsim
