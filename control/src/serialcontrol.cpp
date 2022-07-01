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
using json = nlohmann::json;

using namespace std::literals;

//*********************serialcontrol*****************************************

serialcontrol::serialcontrol(std::string filename){
    std::cout << "starte startup..." << std::endl;
    read_config(filename);
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
    
    auto [message, serialError] = rs232_obj->ReadUntil({'Y'}, 1ms);
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
        dec = dec / 260;    //different divisors needed; max brake is -0.98, -1.0 can only be archieved via emergency_brake

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

    auto dat = data_json["comport"];
    if(!dat.is_string()){
        #if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
            comport = "/dev/ttyACM0";
            std::cout << "nutze Standard-Port! \"/dev/ttyACM0\"" << std::endl;
        #else
            comport = "\\\\.\\COM3";
            std::cout << "nutze Standard-Port! \"\\\\.\\COM3\"" << std::endl;
        #endif
    }else{
        comport = dat.get<std::string>();
    }

    dat = data_json["baudrate"];
    if(!dat.is_number_integer()){
        baudrate = sakurajin::baud9600;
        std::cout << "nutze Standard-Baud 9600!" << std::endl;
    }else{
        switch (dat.get<int>()){
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
    }

    dat = data_json["channels"];
    if (!dat.is_array()){
        std::cerr <<  "channels is not an array" << std::endl;
        return;
    }
    serial_channels.reserve(dat.size());
    for(auto _dat:dat){
        std::string name{_dat["name"].get<std::string>()};
        int channel{_dat["channel"].get<int>()};
        std::string type{_dat["type"].get<std::string>()};

        serial_channel channel_obj{name, channel, type};
        serial_channels.emplace_back(channel_obj);
    }
}

//*********************serial_channel*****************************************

serial_channel::serial_channel(std::string n, int ch, std::string t): name{n},channel{ch},type{t},value{0} {}
