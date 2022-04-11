#include "serialcontrol.hpp"
#include "rs232.c"
#include "physics/include/physics.hpp"
#include <string>
#include <iostream>
#include <fstream>

using namespace libtrainsim;
using json = nlohmann::json;

//*********************serialcontrol*****************************************
libtrainsim::serial_channels libtrainsim::serialcontrol::port[14] = {};
bool libtrainsim::serialcontrol::emergency_flag;

int serialcontrol::hex2int(char hex)
{
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;
    return -1;
}

int serialcontrol::get_value_analog (char v1, char v2)
{
    int value;
    v1 = hex2int(v1);
        if (v1 == -1) return 999;
    v2 = hex2int(v2);
        if (v2 == -1) return 999;
    value = v1*16 + v2;
    return value;
}

int serialcontrol::get_value_digital (char x)
{
    int value = x - '0';
    return value;
}

int serialcontrol::get_portnumber(char int1, char int2)
{
    int portnumber = (int1-'0')*10 + (int2-'0');
    return portnumber;
}

void serialcontrol::openCOMPort(){
    int bdrate = get_json_data(filename, "baudrate");
    char mode[]={'8','N','1',0};

    cport_nr = get_json_data(filename, "comport");

    if(RS232_OpenComport(cport_nr, bdrate, mode, 0))
    {
        serialcontrol::serialflag = false;
    }
    else
    {
    serialcontrol::serialflag = true;
    std::cout << "Opened comport" << std::endl;
    }
    std::cout << "serial flag: " << serialcontrol::serialflag << std::endl;
}

void serialcontrol::updateSerial(){

    unsigned char buffer[10];
    unsigned char telegram[10];

    int port_number;
    int n, i;
    int count = get_json_data(filename, "count_digital") - 1;
    cport_nr = get_json_data(filename, "comport");

    n = RS232_PollComport(cport_nr, buffer, 1);
    if(n > 0 && buffer [0] == 'X')
    {
      i=0;
      telegram[0] = buffer [0];
      do
      {
        i++;
        while (1)
        {
          n = RS232_PollComport(cport_nr, buffer, 1);
          if (n > 0) break;
        }
        telegram[i] = buffer[0];
        if (i >= 9)
        {
          break;
        }
      } while (buffer[0] != 'Y');

    if (telegram[0] == 'X' && telegram[8] == 'Y')
    {
      if (telegram[1] == 'V'){            //analog
        port_number = get_portnumber(telegram[2], telegram[3]);
        set_serial(port_number + count, get_value_analog(telegram[6], telegram[7]));
      }
      else if (telegram [1] == 'U'){    //digital
        port_number = get_portnumber(telegram[2], telegram[3]);
        set_serial(port_number, get_value_digital(telegram[7]));
      }
    }
    }
    #ifdef _WIN32
        Sleep(10);
    #else
        usleep(10000);  /* sleep for 10 milliSeconds */
    #endif
}

void serialcontrol::startup(){

    std::cout << "starte startup..." << std::endl;

    for (int i = 0 ; i < 15; i++){
        port[i].channel_value = 0;
        port[i].effective_slvl = 0.0;
    }

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
    libtrainsim::serialcontrol serial;

    double acc, dec;
    
    if (get_serial(get_json_data(serial.filename, "emergency_brake")) == 1){
        emergency_flag = 1;         
        return -1.0;
    }else if (emergency_flag == 1){
            if (get_serial(get_json_data(serial.filename, "analog_drive") + get_json_data(serial.filename, "count_digital") - 1) < 2){
                emergency_flag = 0;
            }else {
                return 0.0;
            }
    }else if (emergency_flag == 0){
        acc = get_serial(get_json_data(serial.filename, "analog_drive") + get_json_data(serial.filename, "count_digital") - 1);
        dec = get_serial(get_json_data(serial.filename, "analog_brake") + get_json_data(serial.filename, "count_digital") - 1);

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