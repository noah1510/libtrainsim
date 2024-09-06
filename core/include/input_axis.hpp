#pragma once

#include "clampedVariable.hpp"

namespace libtrainsim::core {
    LIBTRAINSIM_EXPORT_MACRO typedef clampedVariable<long double, static_cast<long double>(-1.0), static_cast<long double>(1.0)> input_axis;
} // namespace libtrainsim::core
