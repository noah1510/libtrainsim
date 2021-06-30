#include "input_axis.hpp"

#include <cstdint>
#include <algorithm>

using namespace libtrainsim;

core::input_axis::input_axis(long double _val){
    set(_val);
};

void core::input_axis::operator=(long double newVal){
    set(newVal);
}

void core::input_axis::set(long double newVal){
    value = std::clamp<long double>(newVal,-1.0,1.0);
}

auto core::input_axis::get() const -> long double{
    return value;
}


void core::input_axis::operator+=(long double val){
    set(get()+val);
}

void core::input_axis::operator-=(long double val){
    set(get()-val);
}

void core::input_axis::operator+=(const input_axis& other){
    set(get()+other.get());
}

void core::input_axis::operator-=(const input_axis& other){
    set(get()-other.get());
}



auto core::input_axis::operator+(long double val) const -> input_axis{
    return input_axis(this->value + val);
}

auto core::input_axis::operator-(long double val) const -> input_axis{
    return input_axis(this->value - val);
}

auto core::input_axis::operator+(const input_axis& other) const -> input_axis{
    return input_axis(this->value + other.value);
}

auto core::input_axis::operator-(const input_axis& other) const -> input_axis{
    return input_axis(this->value - other.value);
}


void core::input_axis::operator*=(long double val){
    set(get()*val);
}

void core::input_axis::operator/=(long double val){
    set(get()/val);
}

void core::input_axis::operator*=(const input_axis& other){
    set(get()*other.get());
}

void core::input_axis::operator/=(const input_axis& other){
    set(get()/other.get());
}



auto core::input_axis::operator*(long double val) const -> input_axis{
    return input_axis(this->value * val);
}

auto core::input_axis::operator/(long double val) const -> input_axis{
    return input_axis(this->value / val);
}

auto core::input_axis::operator*(const input_axis& other) const -> input_axis{
    return input_axis(this->value * other.value);
}

auto core::input_axis::operator/(const input_axis& other) const -> input_axis{
    return input_axis(this->value / other.value);
}


bool core::input_axis::operator<(long double val) const{
    return this->value < val;
}

bool core::input_axis::operator>(long double val) const{
    return this->value > val;
}

bool core::input_axis::operator<=(long double val) const{
    return this->value <= val;
}

bool core::input_axis::operator>=(long double val) const{
    return this->value >= val;
}

bool core::input_axis::operator==(long double val) const{
    return this->value == val;
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

bool core::input_axis::operator==(const input_axis& other) const{
    return this->value == other.value;
}
