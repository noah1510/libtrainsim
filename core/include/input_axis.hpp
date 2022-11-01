#pragma once

#include "clampedVariable.hpp"

namespace libtrainsim {
    namespace core {
        /**
         * @brief The input axis is a type to confine an input between -1.0 and 1.0.
         * The input axis is now a child class of the clampedVariable since they behave the same way.
         * It inherits all operatator overloads from the parent class, so in most cases it
         * can be used like a normal double just with contricted values.
         */
        class input_axis : public clampedVariable<long double>{
        private:
            
        public:
            /**
            * @brief Construct a new input axis with a default value.
            * 
            * @param _val A default value can be passed to the contructor.
            */
            input_axis(long double _val = 0.0);
        };
    }
}
