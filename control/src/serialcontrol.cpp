#include "serialcontrol.hpp"

using namespace std::literals;
using namespace SimpleGFX;

libtrainsim::control::serialcontrol::serialcontrol(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _config)
    : config(std::move(_config)) {
    std::scoped_lock lock{accessMutex};

    *config->getLogger() << debug << "starting serialcontrol";
    try {
        read_config(config->getSerialConfigLocation());
    } catch (...) {
        std::throw_with_nested(std::runtime_error("could not read serial config"));
    }

    rs232_obj = std::make_unique<sakurajin::RS232>(comport, baudrate);
    if (!rs232_obj->IsAvailable()) {
        *config->getLogger() << warning << "serialPort" << rs232_obj->GetDeviceName()
                             << " is not available! Check the config and if the device is connected to " << comport;
        return;
    }

    isConnected = true;

    updateLoop = std::async(std::launch::async, [&]() {
        do {
            auto [message, serialError] = rs232_obj->ReadUntil({'Y'}, 100ms);
            if (serialError < 0) {
                continue;
            }

            serial_channel channel;
            try {
                channel = decodeTelegram(message);
            } catch (const std::exception& e) {
                config->getLogger()->logException(e, false);
                continue;
            }

            if (registered) {
                SimpleGFX::inputEvent e;
                e.name       = channel.name;
                e.originName = "serialcontrol";
                e.amount     = static_cast<double>(channel.value);

                if (channel.isAnalog) {
                    e.inputType = SimpleGFX::inputAction::update;
                } else {
                    if (channel.value == 0) {
                        e.inputType = SimpleGFX::inputAction::release;
                    } else {
                        e.inputType = SimpleGFX::inputAction::press;
                    }
                }

                raiseEvent(e);
            }
        } while (IsConnected());
    });
    rs232_obj.reset();

    *config->getLogger() << normal << "serialcontrol fully started";
}

libtrainsim::control::serialcontrol::~serialcontrol() {
    if (IsConnected()) {
        connectedMutex.lock();
        isConnected = false;
        connectedMutex.unlock();
    }

    if (updateLoop.valid()) {
        updateLoop.wait();
        updateLoop.get();
    }
}


int libtrainsim::control::serialcontrol::hex2int(char hex) const {
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    }
    if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 10;
    }
    return -1;
}

libtrainsim::control::serial_channel libtrainsim::control::serialcontrol::decodeTelegram(const std::string& telegram) const {

    // 8 digits and \0 at the end
    if (telegram.length() != 9) {
        throw std::invalid_argument("invalid length of telegram");
    }

    // invalid begin of message
    if (telegram.at(0) != 'X') {
        throw std::invalid_argument("invalid begin of telegram");
    }

    // invalid end of message
    if (telegram.at(8) != 'Y') {
        throw std::invalid_argument("invalid end of telegram");
    }

    // decode the type of the telegram
    bool isDigital;
    auto type = telegram.at(1);
    if (type == 'V') {
        isDigital = false;
    } else if (type == 'U') {
        isDigital = true;
    } else {
        throw std::invalid_argument("invalid type of telegram");
    }

    // decode the port number of the telegram
    uint8_t port;
    port = (telegram.at(2) - '0') * 10 + (telegram.at(3) - '0');

    // decode the value of the telegram
    uint8_t value;
    if (isDigital) {
        value = telegram.at(7) - '0';
    } else {
        auto v1 = hex2int(telegram.at(6));
        auto v2 = hex2int(telegram.at(7));

        if (v1 < 0 || v2 < 0) {
            throw std::invalid_argument("invalid value of telegram");
        }

        value = v1 * 16 + v2;
    }

    for (auto channel : serial_channels) {
        if (channel.channel == port && channel.isAnalog == !isDigital) {
            channel.value = value;
            return channel;
        }
    }

    throw std::invalid_argument("somehow got an unidentified channel");
}

bool libtrainsim::control::serialcontrol::IsConnected() {
    std::shared_lock lock{accessMutex};
    return isConnected;
}

void libtrainsim::control::serialcontrol::read_config(const std::filesystem::path& filename) {
    if (!std::filesystem::exists(filename)) {
        throw std::runtime_error("serial config file does not exist! " + filename.string());
    }

    if (filename.extension() != ".json") {
        throw std::runtime_error("serial config has wrong file type " + filename.extension().string());
    }

    nlohmann::json data_json;

    auto in = std::ifstream(filename);
    in >> data_json;

    try {
        comport = SimpleGFX::helper::getJsonField<std::string>(data_json, "comport");
    } catch (...) {
        std::throw_with_nested("error reading the comport");
    }

    try {
        auto baud = SimpleGFX::helper::getJsonField<int>(data_json, "baudrate");
        switch (baud) {
            case 110:
                baudrate = sakurajin::baud110;
                break;
            case 300:
                baudrate = sakurajin::baud300;
                break;
            case 600:
                baudrate = sakurajin::baud600;
                break;
            case 1200:
                baudrate = sakurajin::baud1200;
                break;
            case 2400:
                baudrate = sakurajin::baud2400;
                break;
            case 4800:
                baudrate = sakurajin::baud4800;
                break;
            case 9600:
                baudrate = sakurajin::baud9600;
                break;
            case 19200:
                baudrate = sakurajin::baud19200;
                break;
            case 38400:
                baudrate = sakurajin::baud38400;
                break;
            case 57600:
                baudrate = sakurajin::baud57600;
                break;
            case 115200:
                baudrate = sakurajin::baud115200;
                break;
            default:
                baudrate = sakurajin::baud9600;
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error reading baudrate"));
    }

    try {
        auto dat = SimpleGFX::helper::getJsonField(data_json, "channels");
        if (!dat.is_array()) {
            throw std::runtime_error("channels is not an array");
        }
        serial_channels.reserve(dat.size());
        for (const auto& _dat : dat) {
            auto name    = SimpleGFX::helper::getJsonField<std::string>(_dat, "name");
            auto channel = SimpleGFX::helper::getJsonField<int>(_dat, "channel");
            auto type    = SimpleGFX::helper::getJsonField<std::string>(_dat, "type");
            auto dir     = SimpleGFX::helper::getJsonField<std::string>(_dat, "direction");

            serial_channel channel_obj{name, channel, type == "analog", dir == "input"};
            serial_channels.emplace_back(channel_obj);
        }

    } catch (...) {
        std::throw_with_nested(std::runtime_error("error parsing channels"));
    }
}

libtrainsim::control::serial_channel::serial_channel(const std::string& n, int ch, bool t, bool dir)
    : name{n},
      channel{ch},
      isAnalog{t},
      value{0},
      directionInput{dir} {}
