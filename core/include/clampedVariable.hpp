#pragma once

#include <cmath>
#include <algorithm>

namespace libtrainsim{
    namespace core{
        template <typename T, T lower, T higher>
        class clampedVariable{
        private:
            T value = 0.0;
        public:
            /**
            * @brief Construct a new input axis with a default value.
            * 
            * @param _val A default value can be passed to the contructor.
            */
            clampedVariable(T _val = 0.0) noexcept;
            
            /**
             * @brief Just assign any double to it and the value will be automatically clamped.
             * 
             * @param newVal the vaule the axis should have now.
             */
            void operator=(T newVal) noexcept;

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

template<typename T, T lower, T higher>
libtrainsim::core::clampedVariable<T, lower, higher>::clampedVariable ( T _val ) noexcept{
    set(_val);
}

template<typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator= ( T newVal ) noexcept {
    set(newVal);
}

template<typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::set ( T newVal ) noexcept {
    value = std::clamp(newVal, lower, higher);
}

template<typename T, T lower, T higher>
T libtrainsim::core::clampedVariable<T, lower, higher>::get() const noexcept {
    return value;
}

template<typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator+=(T val) noexcept{
    set(get()+val);
}

template<typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator-=(T val) noexcept{
    set(get()-val);
}

template<typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator+=(const clampedVariable& other) noexcept{
    set(get()+other.get());
}

template<typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator-=(const clampedVariable& other) noexcept{
    set(get()-other.get());
}



template<typename T, T lower, T higher>
auto libtrainsim::core::clampedVariable<T, lower, higher>::operator+(T val) const noexcept -> clampedVariable{
    return clampedVariable(this->value + val);
}

template<typename T, T lower, T higher>
auto libtrainsim::core::clampedVariable<T, lower, higher>::operator-(T val) const noexcept -> clampedVariable{
    return clampedVariable(this->value - val);
}

template<typename T, T lower, T higher>
auto libtrainsim::core::clampedVariable<T, lower, higher>::operator+(const clampedVariable& other) const noexcept -> clampedVariable{
    return clampedVariable(this->value + other.value);
}

template<typename T, T lower, T higher>
auto libtrainsim::core::clampedVariable<T, lower, higher>::operator-(const clampedVariable& other) const noexcept -> clampedVariable{
    return clampedVariable(this->value - other.value);
}


template<typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator*=(T val) noexcept{
    set(get()*val);
}

template<typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator/=(T val) noexcept{
    set(get()/val);
}

template<typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator*=(const clampedVariable& other) noexcept{
    set(get()*other.get());
}

template<typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator/=(const clampedVariable& other) noexcept{
    set(get()/other.get());
}



template<typename T, T lower, T higher>
auto libtrainsim::core::clampedVariable<T, lower, higher>::operator*(T val) const noexcept -> clampedVariable{
    return clampedVariable(this->value * val);
}

template<typename T, T lower, T higher>
auto libtrainsim::core::clampedVariable<T, lower, higher>::operator/(T val) const noexcept -> clampedVariable{
    return clampedVariable(this->value / val);
}

template<typename T, T lower, T higher>
auto libtrainsim::core::clampedVariable<T, lower, higher>::operator*(const clampedVariable& other) const noexcept -> clampedVariable{
    return clampedVariable(this->value * other.value);
}

template<typename T, T lower, T higher>
auto libtrainsim::core::clampedVariable<T, lower, higher>::operator/(const clampedVariable& other) const noexcept -> clampedVariable{
    return clampedVariable(this->value / other.value);
}


template<typename T, T lower, T higher>
bool libtrainsim::core::clampedVariable<T, lower, higher>::operator<(T val) const noexcept{
    return this->value < val;
}

template<typename T, T lower, T higher>
bool libtrainsim::core::clampedVariable<T, lower, higher>::operator>(T val) const noexcept{
    return this->value > val;
}

template<typename T, T lower, T higher>
bool libtrainsim::core::clampedVariable<T, lower, higher>::operator<=(T val) const noexcept{
    return this->value <= val;
}

template<typename T, T lower, T higher>
bool libtrainsim::core::clampedVariable<T, lower, higher>::operator>=(T val) const noexcept{
    return this->value >= val;
}

template<typename T, T lower, T higher>
bool libtrainsim::core::clampedVariable<T, lower, higher>::operator==(T val) const noexcept{
    return this->value == val;
}


template<typename T, T lower, T higher>
bool libtrainsim::core::clampedVariable<T, lower, higher>::operator<(const clampedVariable& other) const noexcept{
    return this->value < other.value;
}

template<typename T, T lower, T higher>
bool libtrainsim::core::clampedVariable<T, lower, higher>::operator>(const clampedVariable& other) const noexcept{
    return this->value > other.value;
}

template<typename T, T lower, T higher>
bool libtrainsim::core::clampedVariable<T, lower, higher>::operator<=(const clampedVariable& other) const noexcept{
    return this->value <= other.value;
}

template<typename T, T lower, T higher>
bool libtrainsim::core::clampedVariable<T, lower, higher>::operator>=(const clampedVariable& other) const noexcept{
    return this->value >= other.value;
}

template<typename T, T lower, T higher>
bool libtrainsim::core::clampedVariable<T, lower, higher>::operator==(const clampedVariable& other) const noexcept{
    return this->value == other.value;
}
