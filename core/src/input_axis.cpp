#include "input_axis.hpp"

#include <cstdint>
#include <algorithm>

using namespace libtrainsim;

core::input_axis::input_axis(long double _val):value{_val}{};

void core::input_axis::operator=(long double newVal){
    set(newVal);
}

void core::input_axis::set(long double newVal){
    value = std::clamp<long double>(newVal,-1.0,1.0);
}

auto core::input_axis::get() -> long double const{
    return value;
}


void core::input_axis::operator+=(long double val){
    set(get()+val);
}

void core::input_axis::operator-=(long double val){
    set(get()-val);
}

auto core::input_axis::operator+(const input_axis& other) -> input_axis const{
    return input_axis(this->value + other.value);
}

auto core::input_axis::operator-(const input_axis& other) -> input_axis const{
    return input_axis(this->value - other.value);
}


bool core::input_axis::operator<(const input_axis& other) const{
    return this->value < other.value;
}

bool core::input_axis::operator>(const input_axis& other) const{
    return this->value > other.value;
}

bool core::input_axis::operator<=(const input_axis& other) const{
    return this->value <= other.value;
}

bool core::input_axis::operator>=(const input_axis& other) const{
    return this->value >= other.value;
}

