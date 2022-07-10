#include "serialcontrol.hpp"
#include <rs232.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>

using namespace libtrainsim;
using namespace libtrainsim::control;
using json = nlohmann::json;

using namespace std::literals;

//*********************serialcontrol*****************************************

serialcontrol::serialcontrol(std::string filename){
    std::cout << "starte startup..." << std::endl;
    try{
        read_config(filename);
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not read config"));
    }
    
    rs232_obj = std::make_unique<sakurajin::RS232>(comport, baudrate);    
    if(!rs232_obj->IsAvailable()){
        std::cerr << "serialPort" << rs232_obj->GetDeviceName() << " is not available!" << std::endl;
        return;
    }
    
    isConnected = true;
    emergency_flag = false;
    std::cout << "beende startup..." << std::endl;
};

int serialcontrol::hex2int(char hex) const{
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;
    return -1;
}

void serialcontrol::update(){
    
    auto [message, serialError] = rs232_obj->ReadUntil({'Y'}, 10ms);
    if(serialError < 0){
        return;
    }
    
    auto [port, value, isDigital, decodeError] = decodeTelegram(message);
    if(decodeError < 0){
        return;
    }
    
    set_serial(port, value, !isDigital);
}

std::tuple<uint8_t, uint8_t, bool, int> serialcontrol::decodeTelegram(const std::string& telegram) const{
    
    //8 digits and \0 at the end
    if(telegram.length() != 9){
        return {0,0,false,-1};
    }
    
    //invalid begin of message
    if(telegram.at(0) != 'X'){
        return {0,0,false,-2};
    }
    
    //invalid end of message
    if(telegram.at(8) != 'Y'){
        return {0,0,false,-3};
    }
    
    //decode the type of the telegram
    bool isDigital = false;
    auto type = telegram.at(1);
    if(type == 'V'){
        isDigital = false;
    }else if(type == 'U'){
        isDigital = true;
    }else{
        return {0,0,false,-4};
    }
    
    //decode the port number of the telegram
    uint8_t port = 0;
    port = ( telegram.at(2) - '0') * 10 + (telegram.at(3) - '0');
    
    //decode the value of the telegram
    uint8_t value = 0;
    if(isDigital){
        value = telegram.at(7) - '0';
    }else{
        auto v1 = hex2int(telegram.at(6));
        auto v2 = hex2int(telegram.at(7));
        
        if(v1 < 0 || v2 < 0){
            return {0,0,false,-5};
        }
        
        value = v1*16 + v2;
    }
    
    return {port, value, isDigital, 0};
}

int serialcontrol::get_serial(std::string name){
    for (size_t i = 0; i < serial_channels.size(); i++){
        if (serial_channels[i].name == name){
            return serial_channels[i].value;
        }
    }
    return -1;
}

void serialcontrol::set_serial(int i, int value, bool isAnalog){
    if(isAnalog){
        for (size_t n = 0; n < serial_channels.size(); n++){
            if (serial_channels[n].type == "analog" && serial_channels[n].channel == i){
                serial_channels[n].value = value;
            }
        }
    }else{
        for (size_t n = 0; n < serial_channels.size(); n++){
            if (serial_channels[n].type == "digital" && serial_channels[n].channel == i){
                serial_channels[n].value = value;
            }
        }
    }
}

libtrainsim::core::input_axis serialcontrol::get_slvl(){
    if (get_serial("emergency_brake") == 1){
        emergency_flag = true;         
        return -1.0;
    }else if (get_emergencyflag()){
        if (get_serial("analog_drive") < 2){
            emergency_flag = false;
        }else {
            return 0.0;
        }
    }else if (!get_emergencyflag()){
        double acc = get_serial("analog_drive");
        double dec = get_serial("analog_brake");

        acc = acc / 255;
        dec = dec / 255;

        libtrainsim::core::input_axis slvl = acc - dec;

        return slvl;
    }
    return 0.0;
}

bool serialcontrol::IsConnected(){
    return isConnected;
}

bool serialcontrol::get_emergencyflag(){
    return emergency_flag;
}

void serialcontrol::read_config(std::string filename){

    json data_json;

    auto in = std::ifstream(filename);
    in >> data_json;

    try{
        comport = core::Helper::getJsonField<std::string>(data_json,"comport");
    }catch(...){
        std::throw_with_nested("error reading the comport");
    }

    try{
        auto baud = core::Helper::getJsonField<int>(data_json,"baudrate");
        switch (baud){
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
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error reading baudrate"));
    }

    try{
        auto dat = core::Helper::getJsonField(data_json,"channels");
        if (!dat.is_array()){
            throw std::runtime_error("channels is not an array");
        }
        serial_channels.reserve(dat.size());
        for(auto _dat:dat){
            auto name = core::Helper::getJsonField<std::string>(_dat, "name");
            auto channel = core::Helper::getJsonField<int>(_dat, "channel");
            auto type = core::Helper::getJsonField<std::string>(_dat, "type");
            auto dir = core::Helper::getJsonField<std::string>(_dat, "direction");

            serial_channel channel_obj{name, channel, type, dir};
            serial_channels.emplace_back(channel_obj);
        }
        
    }catch(...){
        std::throw_with_nested(std::runtime_error("error parsing channels"));
    }
    
    
}

//*********************serial_channel*****************************************

serial_channel::serial_channel(const std::string& n, int ch, const std::string& t, const std::string& dir): name{n},channel{ch},type{t},value{0},direction{dir} {}
