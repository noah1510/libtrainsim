#pragma once

//the precompiled header for the core module
//it has all the includes for the required external libs
#include "pch/libtrainsim_core_pch.hpp"

#ifndef LIBTRAINSIM_EXPORT_MACRO
    #define LIBTRAINSIM_EXPORT_MACRO
#endif

//generic common types
#include "clampedVariable.hpp"
#include "input_axis.hpp"
#include "version.hpp"

//core trainsim classes
#include "simulator_config.hpp"
#include "track_configuration.hpp"
#include "track_data.hpp"
#include "train_properties.hpp"

