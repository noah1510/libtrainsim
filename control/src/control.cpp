#include "control.hpp"
#include <iostream>

libtrainsim::control::control(void){

}

void libtrainsim::control::hello_impl(){
    std::cout<<"Hello from the control singleton"<<std::endl;
}