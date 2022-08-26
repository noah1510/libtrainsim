#include "input_axis.hpp"

#include <cstdint>
#include <algorithm>

using namespace libtrainsim;

core::input_axis::input_axis(long double _val):clampedVariable<long double>{-1.0,1.0,_val}{}

