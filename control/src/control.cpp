#include "control.hpp"
#include <iostream>

libtrainsim::control::control(void){

}

std::string libtrainsim::control::hello_impl() const{
    return "Hello from the control singleton";
}