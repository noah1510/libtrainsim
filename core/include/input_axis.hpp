#pragma once

#include "clampedVariable.hpp"

namespace libtrainsim {
    namespace core {
        /**
         * @brief The input axis is a type to confine an input between -1.0 and 1.0.
         * All of the needed operators are overloaded, so that it can be a simple replacement
         * for using a double to keep track of the value. All of the operators make sure, that
         * the value is in the expected range.
         * 
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
