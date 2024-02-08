#include "serialcontrol.hpp"

using namespace std::literals;
using namespace SimpleGFX;

libtrainsim::control::serialcontrol::serialcontrol(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _config)
    : config(std::move(_config)) {

    *config->getLogger() << debug << "starting serialcontrol";

    try {
        baudrate = read_config(config->getSerialConfigLocation());
    } catch (...) {
        std::throw_with_nested(std::runtime_error("could not read serial config"));
    }

    config->getInputManager()->registerPoller(*this);

    try{
        auto rs232_log_stream = *config->getLogger() << error;
        rs232_obj             = std::make_unique<sakurajin::RS232>(baudrate, rs232_log_stream);
    }catch(...){
        config->getLogger()->logCurrrentException();
        rs232_obj.reset();
        rs232_obj = nullptr;
    }

    if(rs232_obj->getCurrentDevice() == nullptr){
        *config->getLogger() << error << "no serial port found";
        return;
    }

    if (rs232_obj->getCurrentDevice()->getConnectionStatus() != sakurajin::connectionStatus::connected) {
        *config->getLogger() << error << "could not connect to serial port " << rs232_obj->getCurrentDevice()->getDeviceName();
        return;
    }

    *config->getLogger() << normal << "serialcontrol fully started";
}

sakurajin::Baudrate libtrainsim::control::serialcontrol::read_config(const std::filesystem::path& filename) {
    if (!std::filesystem::exists(filename)) {
        throw std::runtime_error("serial config file does not exist! " + filename.string());
    }

    if (filename.extension() != ".json") {
        throw std::runtime_error("serial config has wrong file type " + filename.extension().string());
    }

    nlohmann::json data_json;

    auto in = std::ifstream(filename);
    in >> data_json;

    sakurajin::Baudrate baudrate;
    try {
        auto baud = SimpleGFX::json::getJsonField<int>(data_json, "baudrate");
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
        std::scoped_lock lock{channelMutex};

        auto dat = SimpleGFX::json::getJsonField(data_json, "channels");
        if (!dat.is_array()) {
            throw std::runtime_error("channels is not an array");
        }
        serial_channels.reserve(dat.size());
        for (const auto& _dat : dat) {
            auto name    = SimpleGFX::json::getJsonField<std::string>(_dat, "name");
            auto channel = SimpleGFX::json::getJsonField<int>(_dat, "channel");
            auto type    = SimpleGFX::json::getJsonField<std::string>(_dat, "type");
            auto dir     = SimpleGFX::json::getJsonField<std::string>(_dat, "direction");

            serial_channel channel_obj{name, channel, type == "analog", dir == "input"};
            serial_channels.emplace_back(channel_obj);
        }

    } catch (...) {
        std::throw_with_nested(std::runtime_error("error parsing channels"));
    }

    return baudrate;
}

libtrainsim::control::serial_channel libtrainsim::control::serialcontrol::decodeTelegram(const std::string& telegram) {

    if (!std::regex_match(telegram, telegramRegex)) {
        throw std::invalid_argument("invalid telegram");
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
        auto val = hex2int(telegram.substr(6, 2));
        if (val < 0) {
            throw std::invalid_argument("invalid value of telegram");
        }
        value = static_cast<uint8_t>(val);
    }

    std::shared_lock lock{channelMutex};
    for (auto channel : serial_channels) {
        if (channel.channel == port && channel.isAnalog == !isDigital) {
            channel.value = value;
            return channel;
        }
    }

    throw std::invalid_argument("somehow got an unidentified channel");
}

libtrainsim::control::serialcontrol::~serialcontrol() {
    rs232_obj.reset();
}

void libtrainsim::control::serialcontrol::operator()(SimpleGFX::eventManager& manager) {
    if (!IsConnected()) {
        return;
    }

    std::vector<std::string> rawTelegrams = rs232_obj->retrieveAllMatches(telegramRegex);
    if (rawTelegrams.empty()) {
        return;
    }

    for (const auto& telegram : rawTelegrams) {
        try {
            auto channel = decodeTelegram(telegram);

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

            manager.raiseEvent(e);
        } catch (const std::exception& e) {
            config->getLogger()->logException(e, false);
            continue;
        }
    }
}

bool libtrainsim::control::serialcontrol::connect() {
    if (rs232_obj == nullptr) {
        try{
            auto rs232_log_stream = *config->getLogger() << error;
            rs232_obj             = std::make_unique<sakurajin::RS232>(baudrate, rs232_log_stream);
            return rs232_obj->getCurrentDevice()->getConnectionStatus() == sakurajin::connectionStatus::connected;
        }catch(...){
            config->getLogger()->logCurrrentException();
            rs232_obj.reset();
            rs232_obj = nullptr;
        }

        return false;
    }
    return rs232_obj->Connect();
}

void libtrainsim::control::serialcontrol::disconnect() {
    if (rs232_obj == nullptr) {
        return;
    }
    rs232_obj->DisconnectAll();
}

bool libtrainsim::control::serialcontrol::IsConnected() {
    if (rs232_obj == nullptr) {
        return false;
    }
    auto currentDevice = rs232_obj->getCurrentDevice();
    if (currentDevice == nullptr) {
        return false;
    }

    return currentDevice->getConnectionStatus() == sakurajin::connectionStatus::connected;
}


libtrainsim::control::serial_channel::serial_channel(const std::string& n, int ch, bool t, bool dir)
    : name{n},
      channel{ch},
      isAnalog{t},
      value{0},
      directionInput{dir} {}
