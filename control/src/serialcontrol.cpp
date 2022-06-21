#include "serialcontrol.hpp"
#include "physics/include/physics.hpp"
#include <rs232.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>

using namespace libtrainsim;
using json = nlohmann::json;

//*********************serialcontrol*****************************************

serialcontrol::serialcontrol(std::string filename){
    std::cout << "starte startup..." << std::endl;

    set_isConnected(false);
    read_config(filename);
    rs232_obj = std::make_unique<sakurajin::RS232>(get_cport(), get_baud());    

    if(!rs232_obj->IsAvailable()){
        std::cerr << "serialPort" << rs232_obj->GetDeviceName() << " is not available!" << std::endl;
        set_isConnected(false);
    }else{
        std::cout << "opened comport" << std::endl;
        set_isConnected(true);
    }
    std::cout << "serial_isConnected: " << get_isConnected() << std::endl;

    emergency_flag = false;
    std::cout << "beende startup..." << std::endl;
};

int serialcontrol::hex2int(char hex){
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;
    return -1;
}

int serialcontrol::get_value_analog (char v1, char v2){
    int value;
    v1 = hex2int(v1);
        if (v1 == -1) return 999;
    v2 = hex2int(v2);
        if (v2 == -1) return 999;
    value = v1*16 + v2;
    return value;
}

int serialcontrol::get_value_digital (char x){
    int value = x - '0';
    return value;
}

int serialcontrol::get_portnumber(char int1, char int2){
    int portnumber = (int1-'0')*10 + (int2-'0');
    return portnumber;
}

void serialcontrol::update(){

    unsigned char telegram[10];

    int port_number;
    int i;

    auto buffer = rs232_obj->ReadNextChar();
    if(std::get<0>(buffer) == 'X'){
      i=0;
      telegram[0] = std::get<0>(buffer);
      do{
        i++;
        buffer = rs232_obj->ReadNextChar();
        telegram[i] = std::get<0>(buffer);
        if (i >= 9) break;
      } while (std::get<0>(buffer) != 'Y');

      if (telegram[0] == 'X' && telegram[8] == 'Y'){
        if (telegram[1] == 'V'){            //analog
            port_number = get_portnumber(telegram[2], telegram[3]);
            set_serial(port_number, get_value_analog(telegram[6], telegram[7]), true);
        } else if (telegram [1] == 'U'){    //digital
            port_number = get_portnumber(telegram[2], telegram[3]);
            set_serial(port_number, get_value_digital(telegram[7]), false);
        }
      }
    }
    std::chrono::milliseconds dura (10);
    std::this_thread::sleep_for(dura);
}

int serialcontrol::get_serial(std::string name){
    for (long long unsigned int i = 0; i < serial_channels.size(); i++){
            if (serial_channels[i].name == name){
                return serial_channels[i].value;
            }
        }
    return -1;
}

void serialcontrol::set_serial(int i, int value, bool isAnalog){
    if(isAnalog == true){
        for (long long unsigned int n = 0; n < serial_channels.size(); n++){
            if (serial_channels[n].type == "analog" && serial_channels[n].channel == i){
                serial_channels[n].value = value;
            }
        }
    }else{
        for (long long unsigned int n = 0; n < serial_channels.size(); n++){
            if (serial_channels[n].type == "digital" && serial_channels[n].channel == i){
                serial_channels[n].value = value;
            }
        }
    }
}

libtrainsim::core::input_axis serialcontrol::get_slvl(){

    libtrainsim::core::input_axis slvl;

    double acc, dec;
    
    if (get_serial("emergency_brake") == 1){
        emergency_flag = true;         
        return -1.0;
    }else if (get_emergencyflag() == 1){
            if (get_serial("analog_drive") < 2){
                emergency_flag = false;
            }else {
                return 0.0;
            }
    }else if (get_emergencyflag() == 0){
        acc = get_serial("analog_drive");
        dec = get_serial("analog_brake");

        acc = acc / 255;
        dec = dec / 260;

        slvl = acc - dec;

        return slvl;
    }
    return 0.0;
}

bool serialcontrol::get_isConnected(){
    return isConnected;
}

void serialcontrol::set_isConnected(bool value){
    isConnected = value;
}

int serialcontrol::get_emergencyflag(){
    return emergency_flag;
}

void serialcontrol::read_config(std::string filename){

    json data_json;

    auto in = std::ifstream(filename);
    in >> data_json;

    auto dat = data_json["comport"];
    if(!dat.is_string()){
        comport = "\\\\.\\COM3";
        std::cout << "nutze Standard-Port!" << std::endl;
    }else{
        comport = dat.get<std::string>();
    }

    dat = data_json["baudrate"];
    if(!dat.is_number_integer()){
        baudrate = sakurajin::baud9600;
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
            case 128000:
                baudrate = sakurajin::baud128000;
                break;
            case 256000:
                baudrate = sakurajin::baud256000;
                break;
            default:
                baudrate = sakurajin::baud9600;
        }
    }

    dat = data_json["channels"];
    if (!dat.is_array() == true){
        std::cerr <<  "channels is not an array" << std::endl;
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

sakurajin::Baudrate serialcontrol::get_baud(){
    return baudrate;
}

std::string serialcontrol::get_cport(){
    return comport;
}

//*********************serial_channel*****************************************

serial_channel::serial_channel(std::string n, int ch, std::string t){
    name = n;
    channel = ch;
    type = t;
    value = 0;
}