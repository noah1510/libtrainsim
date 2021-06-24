#pragma once

namespace libtrainsim {
    namespace core {
        /**
         * @brief The input axis is a type to confine an input between -1.0 and 1.0.
         * All of the needed operators are overloaded, so that it can be a simple replacement
         * for using a double to keep track of the value. All of the operators make sure, that
         * the value is in the expected range.
         * 
         */
        class input_axis{
        private:
            /**
             * @brief The actual value of the variable
             * 
             */
            long double value = 0.0;
            
        public:
            /**
            * @brief Construct a new input axis with a default value.
            * 
            * @param _val A default value can be passed to the contructor.
            */
            input_axis(long double _val = 0.0);
            
            /**
             * @brief Just assign any double to it and the value will be automatically clamped.
             * 
             * @param newVal the vaule the axis should have now.
             */
            void operator=(long double newVal);

            /**
             * @brief This functions is used to set the value to a given value.
             * 
             * @param newVal the vaule the axis should have now.
             */
            void set(long double newVal);
            
            /**
             * @brief This function returns the value of the axis.
             * 
             * @return long double the current value of the variable. 
             */
            auto get() const -> long double;
            

            void operator+=(long double val);
            void operator-=(long double val);
            void operator+=(const input_axis& other);
            void operator-=(const input_axis& other);
            

            auto operator+(long double val) const -> input_axis;
            auto operator-(long double val) const -> input_axis;
            auto operator+(const input_axis& other) const -> input_axis;
            auto operator-(const input_axis& other) const -> input_axis;


            void operator*=(long double val);
            void operator/=(long double val);
            void operator*=(const input_axis& other);
            void operator/=(const input_axis& other);
            

            auto operator*(long double val) const -> input_axis;
            auto operator/(long double val) const -> input_axis;
            auto operator*(const input_axis& other) const -> input_axis;
            auto operator/(const input_axis& other) const -> input_axis;
            

            bool operator<(long double val) const;
            bool operator>(long double val) const;
            bool operator<=(long double val) const;
            bool operator>=(long double val) const;
            bool operator==(long double val) const;

            bool operator<(const input_axis& other) const;
            bool operator>(const input_axis& other) const;
            bool operator<=(const input_axis& other) const;
            bool operator>=(const input_axis& other) const;
            bool operator==(const input_axis& other) const;
        };
    }
}
