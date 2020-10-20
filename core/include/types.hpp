/**
 * @file types.hpp
 * @author Noah Kirschmann (noah.kirschmann@mnd.thm.de)
 * @brief This file contains basic types for use throughout libtrainsim.
 * @version 0.4.0
 * @date 2020-10-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */
 
#pragma once

/**
 * @brief This namespace contains all the core somponents of libtrainsim.
 * 
 */
namespace libtrainsim::core{

    /**
     * @brief this enum maps readable keywords to the values of the keys.
     * 
     */
    enum keyboard_keys{
        KEY_ESCAPE = 27,
    };

    /**
     * @brief This enum maps action names to values for use with the control
     * 
     */
    enum actions{
        ACTION_CLOSE = 0,
        ACTION_ACCELERATE = 1,
        ACTION_BREAK = 2,
    };

}
