#pragma once

#include <cmath>
#include <algorithm>
#include "helper.hpp"

namespace libtrainsim{
    namespace core{
        template <typename T>
        class clampedVariable{
        private:
            T value = 0.0;
            const T lower;
            const T higher;
        public:
            /**
            * @brief Construct a new input axis with a default value.
            * 
            * @param _val A default value can be passed to the contructor.
            */
            clampedVariable(T _lower, T _higher, T _val = 0.0) noexcept;
            
            
            /**
             * @brief Just assign other axis to it and the value will be automatically clamped.
             * 
             * @param other the value the axis should have now.
             */
            clampedVariable(const clampedVariable<T>& other) noexcept;
            
            /**
             * @brief Just assign any double to it and the value will be automatically clamped.
             * 
             * @param newVal the value the axis should have now.
             */
            void operator=(T newVal) noexcept;
            
            /**
             * @brief Just assign other axis to it and the value will be automatically clamped.
             * 
             * @param other the value the axis should have now.
             */
            void operator=(const clampedVariable<T>& other) noexcept;

            /**
             * @brief This functions is used to set the value to a given value.
             * 
             * @param newVal the vaule the axis should have now.
             */
            void set(T newVal) noexcept;
            
            /**
             * @brief This function returns the value of the axis.
             * 
             * @return T the current value of the variable. 
             */
            T get() const noexcept;
            
            bool isRoughly(const T& val) const noexcept;
            bool isRoughly(const clampedVariable<T>& other) const noexcept;

            void operator+=(T val) noexcept;
            void operator-=(T val) noexcept;
            void operator+=(const clampedVariable& other) noexcept;
            void operator-=(const clampedVariable& other) noexcept;
            

            auto operator+(T val) const noexcept -> clampedVariable;
            auto operator-(T val) const noexcept -> clampedVariable;
            auto operator+(const clampedVariable& other) const noexcept -> clampedVariable;
            auto operator-(const clampedVariable& other) const noexcept -> clampedVariable;


            void operator*=(T val) noexcept;
            void operator/=(T val) noexcept;
            void operator*=(const clampedVariable& other) noexcept;
            void operator/=(const clampedVariable& other) noexcept;
            

            auto operator*(T val) const noexcept -> clampedVariable;
            auto operator/(T val) const noexcept -> clampedVariable;
            auto operator*(const clampedVariable& other) const noexcept -> clampedVariable;
            auto operator/(const clampedVariable& other) const noexcept -> clampedVariable;
            

            bool operator<(T val) const noexcept;
            bool operator>(T val) const noexcept;
            bool operator<=(T val) const noexcept;
            bool operator>=(T val) const noexcept;
            bool operator==(T val) const noexcept;

            bool operator<(const clampedVariable& other) const noexcept;
            bool operator>(const clampedVariable& other) const noexcept;
            bool operator<=(const clampedVariable& other) const noexcept;
            bool operator>=(const clampedVariable& other) const noexcept;
            bool operator==(const clampedVariable& other) const noexcept;
        };
    }
}

template<typename T>
libtrainsim::core::clampedVariable<T>::clampedVariable ( T _lower, T _higher, T _val ) noexcept: lower{_lower}, higher{_higher} {
    set(_val);
}

template<typename T>
libtrainsim::core::clampedVariable<T>::clampedVariable ( const clampedVariable<T>& other ) noexcept : clampedVariable<T>{other.lower, other.higher, other.get()} {}


template<typename T>
void libtrainsim::core::clampedVariable<T>::operator= ( T newVal ) noexcept {
    set(newVal);
}

template<typename T>
void libtrainsim::core::clampedVariable<T>::operator= ( const clampedVariable<T>& other ) noexcept {
    set(other.get());
}


template<typename T>
bool libtrainsim::core::clampedVariable<T>::isRoughly ( const T& val ) const noexcept {
    return libtrainsim::core::Helper::isRoughly<T>(value, val);
}

template<typename T> bool libtrainsim::core::clampedVariable<T>::isRoughly ( const clampedVariable<T>& other ) const noexcept {
    return isRoughly(other.get());
}

template<typename T>
void libtrainsim::core::clampedVariable<T>::set ( T newVal ) noexcept {
    value = std::clamp(newVal, lower, higher);
}

template<typename T>
T libtrainsim::core::clampedVariable<T>::get() const noexcept {
    return value;
}

template<typename T>
void libtrainsim::core::clampedVariable<T>::operator+=(T val) noexcept{
    set(get()+val);
}

template<typename T>
void libtrainsim::core::clampedVariable<T>::operator-=(T val) noexcept{
    set(get()-val);
}

template<typename T>
void libtrainsim::core::clampedVariable<T>::operator+=(const clampedVariable& other) noexcept{
    set(get()+other.get());
}

template<typename T>
void libtrainsim::core::clampedVariable<T>::operator-=(const clampedVariable& other) noexcept{
    set(get()-other.get());
}



template<typename T>
auto libtrainsim::core::clampedVariable<T>::operator+(T val) const noexcept -> clampedVariable{
    return clampedVariable(this->lower,this->higher,this->value + val);
}

template<typename T>
auto libtrainsim::core::clampedVariable<T>::operator-(T val) const noexcept -> clampedVariable{
    return clampedVariable(this->lower,this->higher,this->value - val);
}

template<typename T>
auto libtrainsim::core::clampedVariable<T>::operator+(const clampedVariable& other) const noexcept -> clampedVariable{
    return clampedVariable(this->lower,this->higher,this->value + other.value);
}

template<typename T>
auto libtrainsim::core::clampedVariable<T>::operator-(const clampedVariable& other) const noexcept -> clampedVariable{
    return clampedVariable(this->lower,this->higher,this->value - other.value);
}


template<typename T>
void libtrainsim::core::clampedVariable<T>::operator*=(T val) noexcept{
    set(get()*val);
}

template<typename T>
void libtrainsim::core::clampedVariable<T>::operator/=(T val) noexcept{
    set(get()/val);
}

template<typename T>
void libtrainsim::core::clampedVariable<T>::operator*=(const clampedVariable& other) noexcept{
    set(get()*other.get());
}

template<typename T>
void libtrainsim::core::clampedVariable<T>::operator/=(const clampedVariable& other) noexcept{
    set(get()/other.get());
}



template<typename T>
auto libtrainsim::core::clampedVariable<T>::operator*(T val) const noexcept -> clampedVariable{
    return clampedVariable(this->lower,this->higher,this->value * val);
}

template<typename T>
auto libtrainsim::core::clampedVariable<T>::operator/(T val) const noexcept -> clampedVariable{
    return clampedVariable(this->lower,this->higher,this->value / val);
}

template<typename T>
auto libtrainsim::core::clampedVariable<T>::operator*(const clampedVariable& other) const noexcept -> clampedVariable{
    return clampedVariable(this->lower,this->higher,this->value * other.value);
}

template<typename T>
auto libtrainsim::core::clampedVariable<T>::operator/(const clampedVariable& other) const noexcept -> clampedVariable{
    return clampedVariable(this->lower,this->higher,this->value / other.value);
}


template<typename T>
bool libtrainsim::core::clampedVariable<T>::operator<(T val) const noexcept{
    return this->value < val;
}

template<typename T>
bool libtrainsim::core::clampedVariable<T>::operator>(T val) const noexcept{
    return this->value > val;
}

template<typename T>
bool libtrainsim::core::clampedVariable<T>::operator<=(T val) const noexcept{
    return this->value <= val;
}

template<typename T>
bool libtrainsim::core::clampedVariable<T>::operator>=(T val) const noexcept{
    return this->value >= val;
}

template<typename T>
bool libtrainsim::core::clampedVariable<T>::operator==(T val) const noexcept{
    return this->value == val;
}


template<typename T>
bool libtrainsim::core::clampedVariable<T>::operator<(const clampedVariable& other) const noexcept{
    return this->value < other.value;
}

template<typename T>
bool libtrainsim::core::clampedVariable<T>::operator>(const clampedVariable& other) const noexcept{
    return this->value > other.value;
}

template<typename T>
bool libtrainsim::core::clampedVariable<T>::operator<=(const clampedVariable& other) const noexcept{
    return this->value <= other.value;
}

template<typename T>
bool libtrainsim::core::clampedVariable<T>::operator>=(const clampedVariable& other) const noexcept{
    return this->value >= other.value;
}

template<typename T>
bool libtrainsim::core::clampedVariable<T>::operator==(const clampedVariable& other) const noexcept{
    return this->value == other.value;
}
