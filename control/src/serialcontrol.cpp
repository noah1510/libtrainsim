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

serialcontrol::serialcontrol(){
    serial_channels_obj = std::make_unique<serial_channels>();
    serial_channels_obj->read_config();
    //std::string serialPort = "\\\\.\\COM3";
    rs232_obj = std::make_unique<sakurajin::RS232>(serial_channels_obj->get_cport(), serial_channels_obj->get_baud());
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
    int count = serial_channels_obj->get_config("count_digital") - 1;

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
    
    if (get_serial(serial_channels_obj->get_config("emergency_brake")) == 1){
        set_emergencyflag(1);         
        return -1.0;
    }else if (get_emergencyflag() == 1){
            if (get_serial(serial_channels_obj->get_config("analog_drive") + serial_channels_obj->get_config("count_digital") - 1) < 2){
                set_emergencyflag(0);
            }else {
                return 0.0;
            }
    }else if (get_emergencyflag() == 0){
        acc = get_serial(serial_channels_obj->get_config("analog_drive") + serial_channels_obj->get_config("count_digital") - 1);
        dec = get_serial(serial_channels_obj->get_config("analog_brake") + serial_channels_obj->get_config("count_digital") - 1);

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

void serial_channels::read_config(){

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

    dat=data_json["analog_drive"];
    if(!dat.is_number_integer()){
        analog_drive = 1;
        std::cout << "nutze Standard-Wert für analog_drive!" << std::endl;
    }else{
        analog_drive = dat.get<int>();
    }

    dat=data_json["analog_brake"];
    if(!dat.is_number_integer()){
        analog_brake = 2;
        std::cout << "nutze Standard-Wert für analog_brake!" << std::endl;
    }else{
        analog_brake = dat.get<int>();
    }

    dat=data_json["count_digital"];
    if(!dat.is_number_integer()){
        count_digital = 12;
        std::cout << "nutze Standard-Wert für count_digital!" << std::endl;
    }else{
        count_digital = dat.get<int>();
    }

    dat=data_json["drivemode_r"];
    if(!dat.is_number_integer()){
        drivemode_r = 0;
        std::cout << "nutze Standard-Wert für drivemode_r!" << std::endl;
    }else{
        drivemode_r = dat.get<int>();
    }

    dat=data_json["drivemode_0"];
    if(!dat.is_number_integer()){
        drivemode_0 = 1;
        std::cout << "nutze Standard-Wert für drivemode_0!" << std::endl;
    }else{
        drivemode_0 = dat.get<int>();
    }

    dat=data_json["drivemode_x"];
    if(!dat.is_number_integer()){
        drivemode_x = 2;
        std::cout << "nutze Standard-Wert für drivemode_x!" << std::endl;
    }else{
        drivemode_x = dat.get<int>();
    }

    dat=data_json["drivemode_v"];
    if(!dat.is_number_integer()){
        drivemode_v = 3;
        std::cout << "nutze Standard-Wert für drivemode_v!" << std::endl;
    }else{
        drivemode_v = dat.get<int>();
    }

    dat=data_json["sifa"];
    if(!dat.is_number_integer()){
        sifa = 4;
        std::cout << "nutze Standard-Wert für sifa!" << std::endl;
    }else{
        sifa = dat.get<int>();
    }

    dat=data_json["n_max"];
    if(!dat.is_number_integer()){
        n_max = 5;
        std::cout << "nutze Standard-Wert für n_max!" << std::endl;
    }else{
        n_max = dat.get<int>();
    }

    dat=data_json["digital_drive"];
    if(!dat.is_number_integer()){
        digital_drive = 6;
        std::cout << "nutze Standard-Wert für digital_drive!" << std::endl;
    }else{
        digital_drive = dat.get<int>();
    }

    dat=data_json["digital_brake"];
    if(!dat.is_number_integer()){
        digital_brake = 7;
        std::cout << "nutze Standard-Wert für digital_brake!" << std::endl;
    }else{
        digital_brake = dat.get<int>();
    }

    dat=data_json["emergency_brake"];
    if(!dat.is_number_integer()){
        emergency_brake = 8;
        std::cout << "nutze Standard-Wert für emergency_brake!" << std::endl;
    }else{
        emergency_brake = dat.get<int>();
    }

    dat=data_json["door_r"];
    if(!dat.is_number_integer()){
        door_r = 9;
        std::cout << "nutze Standard-Wert für door_r!" << std::endl;
    }else{
        door_r = dat.get<int>();
    }

    dat=data_json["door_l"];
    if(!dat.is_number_integer()){
        door_l = 10;
        std::cout << "nutze Standard-Wert für door_l!" << std::endl;
    }else{
        door_l = dat.get<int>();
    }

    dat=data_json["door_release"];
    if(!dat.is_number_integer()){
        door_release = 11;
        std::cout << "nutze Standard-Wert für door_release!" << std::endl;
    }else{
        door_release = dat.get<int>();
    }
}

int serial_channels::get_config(std::string value){
    if(value == "analog_drive"){
        return analog_drive;
    }
    if(value == "analog_brake"){
        return analog_brake;
    }
    if(value == "count_digital"){
        return count_digital;
    }
    if(value == "drivemode_r"){
        return drivemode_r;
    }
    if(value == "drivemode_0"){
        return drivemode_0;
    }
    if(value == "drivemode_x"){
        return drivemode_x;
    }
    if(value == "drivemode_v"){
        return drivemode_v;
    }
    if(value == "sifa"){
        return sifa;
    }
    if(value == "n_max"){
        return n_max;
    }
    if(value == "digital_drive"){
        return digital_drive;
    }
    if(value == "digital_brake"){
        return digital_brake;
    }
    if(value == "emergency_brake"){
        return emergency_brake;
    }
    if(value == "door_r"){
        return door_r;
    }
    if(value == "door_l"){
        return door_l;
    }
    if(value == "door_release"){
        return door_release;
    }
    return 99;
}

sakurajin::Baudrate serial_channels::get_baud(){
    return baudrate;
}

std::string serial_channels::get_cport(){
    return comport;
}