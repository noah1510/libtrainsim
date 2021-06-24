#pragma once

namespace libtrainsim {
    namespace core {
        class input_axis{
        private:
            long double value = 0.0;
            
        public:
            input_axis(long double _val = 0.0);
            
            void operator=(long double newVal);
            void set(long double newVal);
            
            auto get() -> long double const;
            
            void operator+=(long double val);
            void operator-=(long double val);
            
            auto operator+(const input_axis& other) -> input_axis const;
            auto operator-(const input_axis& other) -> input_axis const;
            
            bool operator<(const input_axis& other) const;
            bool operator>(const input_axis& other) const;
            bool operator<=(const input_axis& other) const;
            bool operator>=(const input_axis& other) const;
        };
    }
}
