#pragma once

#ifndef LIBTRAINSIM_EXPORT_MACRO
    #define LIBTRAINSIM_EXPORT_MACRO
#endif

namespace libtrainsim {
    namespace core {
        /**
         * @brief a simple type that contricts a variable between two values
         *
         * @tparam T the type that should be clamped between two values
         */
        template <typename T, T lower, T higher>
        class LIBTRAINSIM_EXPORT_MACRO clampedVariable {
          private:
            T value = 0.0;

          public:
            /**
             * @brief Construct a new input axis with a default value.
             *
             * @param _val A default value can be passed to the contructor.
             */
            clampedVariable(auto _val) noexcept { set(static_cast<T>(_val)); }

            /**
             * @brief Just assign other axis to it and the value will be automatically clamped.
             *
             * @param other the value the axis should have now.
             */
            clampedVariable(const clampedVariable<T, lower, higher>& other) = default;

            /**
             * @brief Just assign any double to it and the value will be automatically clamped.
             *
             * @param newVal the value the axis should have now.
             */
            void operator=(auto newVal) noexcept { set(static_cast<T>(newVal)); }

            /**
             * @brief Just assign other axis to it and the value will be automatically clamped.
             *
             * @param other the value the axis should have now.
             */
            clampedVariable<T, lower, higher>& operator=(const clampedVariable<T, lower, higher>& other) noexcept = default;

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

            bool isRoughly(auto val) const noexcept { return SimpleGFX::core::isRoughly<T>(value, static_cast<T>(val)); }
            bool isRoughly(const clampedVariable<T, lower, higher>& other) const noexcept;

            void operator+=(auto val) noexcept { set(get() + static_cast<T>(val)); }
            void operator-=(auto val) noexcept { set(get() - static_cast<T>(val)); }

            void operator+=(const clampedVariable& other) noexcept;
            void operator-=(const clampedVariable& other) noexcept;


            auto operator+(auto val) const noexcept -> clampedVariable { return clampedVariable(this->value + static_cast<T>(val)); }
            auto operator-(auto val) const noexcept -> clampedVariable { return clampedVariable(this->value - static_cast<T>(val)); }
            auto operator+(const clampedVariable& other) const noexcept -> clampedVariable;
            auto operator-(const clampedVariable& other) const noexcept -> clampedVariable;


            void operator*=(auto val) noexcept { set(get() * static_cast<T>(val)); }
            void operator/=(auto val) noexcept { set(get() / static_cast<T>(val)); }
            void operator*=(const clampedVariable& other) noexcept;
            void operator/=(const clampedVariable& other) noexcept;


            auto operator*(auto val) const noexcept -> clampedVariable { return clampedVariable(this->value * static_cast<T>(val)); }
            auto operator/(auto val) const noexcept -> clampedVariable { return clampedVariable(this->value / static_cast<T>(val)); }
            auto operator*(const clampedVariable& other) const noexcept -> clampedVariable;
            auto operator/(const clampedVariable& other) const noexcept -> clampedVariable;

            bool operator==(auto rhs) const { return value == static_cast<T>(rhs); }

            bool operator==(const clampedVariable& rhs) const { return value == rhs.value; }

            auto operator<=>(auto rhs) const { return value <=> static_cast<T>(rhs); }

            auto operator<=>(const clampedVariable& rhs) const { return value <=> rhs.value; }
        };
    } // namespace core
} // namespace libtrainsim


template <typename T, T lower, T higher>
bool libtrainsim::core::clampedVariable<T, lower, higher>::isRoughly(const clampedVariable<T, lower, higher>& other) const noexcept {
    return isRoughly(other.get());
}

template <typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::set(T newVal) noexcept {
    value = std::clamp(newVal, lower, higher);
}

template <typename T, T lower, T higher>
T libtrainsim::core::clampedVariable<T, lower, higher>::get() const noexcept {
    return value;
}


template <typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator+=(const clampedVariable& other) noexcept {
    set(get() + other.get());
}

template <typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator-=(const clampedVariable& other) noexcept {
    set(get() - other.get());
}


template <typename T, T lower, T higher>
auto libtrainsim::core::clampedVariable<T, lower, higher>::operator+(const clampedVariable& other) const noexcept -> clampedVariable {
    return clampedVariable(this->value + other.value);
}

template <typename T, T lower, T higher>
auto libtrainsim::core::clampedVariable<T, lower, higher>::operator-(const clampedVariable& other) const noexcept -> clampedVariable {
    return clampedVariable(this->value - other.value);
}


template <typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator*=(const clampedVariable& other) noexcept {
    set(get() * other.get());
}

template <typename T, T lower, T higher>
void libtrainsim::core::clampedVariable<T, lower, higher>::operator/=(const clampedVariable& other) noexcept {
    set(get() / other.get());
}


template <typename T, T lower, T higher>
auto libtrainsim::core::clampedVariable<T, lower, higher>::operator*(const clampedVariable& other) const noexcept -> clampedVariable {
    return clampedVariable(this->value * other.value);
}

template <typename T, T lower, T higher>
auto libtrainsim::core::clampedVariable<T, lower, higher>::operator/(const clampedVariable& other) const noexcept -> clampedVariable {
    return clampedVariable(this->value / other.value);
}
