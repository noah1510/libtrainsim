#include "input_axis.hpp"

using namespace libtrainsim;

core::input_axis::input_axis(long double _val)
    : clampedVariable<long double>{-1.0, 1.0, _val} {}
