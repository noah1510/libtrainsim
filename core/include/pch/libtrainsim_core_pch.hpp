// The build config for libtrainsim
#include "libtrainsim_config.hpp"

// external libraries
#include "nlohmann/json.hpp"
#include "unit_system.hpp"
#include "simplegfx.hpp"

// standard libraries
#include <algorithm>
#include <cmath>
#include <exception>
#include <filesystem>
#include <future>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#ifndef LIBTRAINSIM_EXPORT_MACRO
    #define LIBTRAINSIM_EXPORT_MACRO
#endif
