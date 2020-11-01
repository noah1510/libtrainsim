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

#include <string>
#include <vector>
#include <sstream>

/**
 * @brief This namespace contains all the core somponents of libtrainsim.
 * 
 */
namespace libtrainsim::core{
    
    /**
     * @brief splits a string into several others
     * 
     * @param s the string to be split
     * @param delimiter the character that determines where to split the string
     * @return std::vector<std::string> a vector containing with the split parts of the string.
     */
    std::vector<std::string> split_string(const std::string& s, char delimiter);
    
    /**
     * @brief a simple class for version checking
     * 
     */
    class version{
    public:
        /**
         * @brief The major version number X.y.z
         * 
         */
        const u_int64_t major;

        /**
         * @brief The minor version number x.Y.z
         * 
         */
        const u_int64_t minor;

        /**
         * @brief The patch version number x.y.Z
         * 
         */
        const u_int64_t patch;
        
        /**
         * @brief Construct a new version object with given major, minor and patch version number.
         * 
         * @param x major
         * @param y minor
         * @param z patch
         */
        version(uint64_t x, uint64_t y, uint64_t z);
        
        /**
         * @brief Construct a new version object from a given string with the format "x.y.z".
         * 
         * @param ver a string in the format manjor.minor.patch
         */
        version(const std::string& ver);
        
        /**
         * @brief returns the version number as "major.minor.patch".
         * 
         * @return std::string The version number as string
         */
        std::string print() const;
        
        /**
         * @brief three way comparison between two version numbers.
         * positive if v1 > v2
         * 0 if v1 == v2
         * negative if v1 < v2
         * 
         * @param v1 the first version
         * @param v2 the second version
         * @return int the result of the comparison
         */
        static int compare(const version& v1, const version& v2){
            if(v1.major > v2.major){return 1;}
            if(v1.major < v2.major){return -1;}
            
            if(v1.minor > v2.minor){return 1;}
            if(v1.minor < v2.minor){return -1;}
            
            if(v1.patch > v2.patch){return 1;}
            if(v1.patch < v2.patch){return -1;}
            
            return 0;
        };
    };
    
    /**
     * @brief the current version of the libtrainsim
     * 
     */
    const version lib_version("0.4.0");

    /**
     * @brief the current version of the json formats
     * 
     */
    const version format_version("0.5.0");

    /**
     * @brief this enum maps readable keywords to the values of the keys.
     * 
     */
    enum keyboard_keys : char{
        KEY_ESCAPE = static_cast<char>(27),
        KEY_CLOSE = static_cast<char>(27),
        KEY_ACCELERATE = 'w',
        KEY_BREAK = 's',
        KEY_OTHER = '\0',
    };

    /**
     * @brief This enum maps action names to values for use with the control
     * 
     */
    enum actions{
        ACTION_OTHER = -2,
        ACTION_NONE = -1,
        ACTION_CLOSE = 0,
        ACTION_ACCELERATE = 1,
        ACTION_BREAK = 2,
    };

    /**
     * @brief clamps the value between two bounds (upper and lower will siwthced if they are in the wrong order).
     * 
     * @tparam T the type of the parameters
     * @param val the value that will be clamped
     * @param min the lower bound for the value
     * @param max the upper bound for the value
     * @return T the clamped value
     */
    template <typename T>
    T clamp(T val, T min, T max){
        if(min > max){
            T temp = max;
            max = min;
            min = temp;
        };

        if(val < min){return min;};
        if(val > max){return max;};
        return val;
    }
}
