#include "serialcontrol.hpp"
#include "physics/include/physics.hpp"
#include <rs232.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

using namespace libtrainsim;
using json = nlohmann::json;

//*********************serialcontrol*****************************************

serialcontrol::serialcontrol(int test){
    std::string serialPort = "\\\\.\\COM3";
    rs232_obj = std::make_unique<sakurajin::RS232>(serialPort, sakurajin::baud9600);
    serialflag = 0;
    emergency_flag = 0;
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

void serialcontrol::updateSerial(){

    unsigned char telegram[10];

    int port_number;
    int i;
    int count = get_json_data(filename, "count_digital") - 1;

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
            set_serial(port_number + count, get_value_analog(telegram[6], telegram[7]));
        } else if (telegram [1] == 'U'){    //digital
            port_number = get_portnumber(telegram[2], telegram[3]);
            set_serial(port_number, get_value_digital(telegram[7]));
        }
      }
    }
    std::chrono::milliseconds dura (10);
    std::this_thread::sleep_for(dura);
}

void serialcontrol::startup(){
    std::cout << "starte startup..." << std::endl; 

    if(!rs232_obj->IsAvailable()){
        std::cerr << "serialPort" << rs232_obj->GetDeviceName() << " is not available!" << std::endl;
        set_serialflag(0);
    }else{
        std::cout << "Opened comport" << std::endl;
        set_serialflag(1);
    }
    std::cout << "serial flag: " << get_serialflag() << std::endl;
  
    for (int i = 0 ; i < 14; i++){
        port[i].channel_value = 0;
        port[i].effective_slvl = 0.0;
    }  
    read_config();
    set_emergencyflag(0);
    std::cout << "beende startup..." << std::endl;
}

int serialcontrol::get_serial(int i){
    return port[i].channel_value;
}

void serialcontrol::set_serial(int i, int value){
    port[i].channel_value = value;
}

libtrainsim::core::input_axis serialcontrol::get_slvl(){

    libtrainsim::core::input_axis slvl;
    std::string datei = "data/production_data/config_serial_input.json";

    double acc, dec;
    
    if (get_serial(get_json_data(datei, "emergency_brake")) == 1){
        set_emergencyflag(1);         
        return -1.0;
    }else if (get_emergencyflag() == 1){
            if (get_serial(get_json_data(datei, "analog_drive") + get_json_data(datei, "count_digital") - 1) < 2){
                set_emergencyflag(0);
            }else {
                return 0.0;
            }
    }else if (get_emergencyflag() == 0){
        acc = get_serial(get_json_data(datei, "analog_drive") + get_json_data(datei, "count_digital") - 1);
        dec = get_serial(get_json_data(datei, "analog_brake") + get_json_data(datei, "count_digital") - 1);

        acc = acc / 255;
        dec = dec / 260;

        slvl = acc - dec;

        return slvl;
    }
    return 0.0;
}

int serialcontrol::get_json_data(std::string filename, std::string name){
    json data_json;
    auto in = std::ifstream(filename);
    in >> data_json;

    auto dat = data_json[name];
    if (!dat.is_number_integer()){
        return 99;
    }
    return dat.get<int>();
}

int serialcontrol::get_serialflag(){
    return serialflag;
}

void serialcontrol::set_serialflag(int value){
    serialflag = value;
}

int serialcontrol::get_emergencyflag(){
    return emergency_flag;
}

void serialcontrol::set_emergencyflag(int value){
    emergency_flag = value;
}

void serialcontrol::read_config(){

}

int serialcontrol::get_config(int value){
    return 1;
}