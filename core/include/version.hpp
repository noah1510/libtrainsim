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

#undef major
#undef minor
#undef patch

/**
 * @brief This namespace contains all of libtrainsim.
 *
 */
namespace libtrainsim {
    /**
     * @brief This namespace contains all the core components of libtrainsim.
     *
     */
    namespace core {

        /**
         * @brief a simple class for version checking
         *
         */
        class LIBTRAINSIM_EXPORT_MACRO version {
          public:
            /**
             * @brief The major version number x.y.z
             *
             */
            const std::tuple<uint64_t, uint64_t, uint64_t> Version;

            /**
             * @brief The major version number X.y.z
             *
             */
            [[nodiscard]]
            uint64_t major() const;

            /**
             * @brief The minor version number x.Y.z
             *
             */
            [[nodiscard]]
            uint64_t minor() const;

            /**
             * @brief The patch version number x.y.Z
             *
             */
            [[nodiscard]]
            uint64_t patch() const;

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
             * @brief Construct a new version object from a given string with the format "x.y.z".
             *
             * @param ver a string in the format manjor.minor.patch
             */
            explicit version(std::tuple<uint64_t, uint64_t, uint64_t> ver);

            /**
             * @brief returns the version number as "major.minor.patch".
             *
             * @return std::string The version number as string
             */
            [[nodiscard]]
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
            static int compare(const version& v1, const version& v2) {
                if (v1.Version > v2.Version) {
                    return 1;
                }
                if (v1.Version < v2.Version) {
                    return -1;
                }

                return 0;
            };

            /*
             * @brief Compare two versions.
             */
            bool operator>(const version& other) const;

            /*
             * @brief Compare two versions.
             */
            bool operator<(const version& other) const;

            /*
             * @brief Compare two versions.
             */
            bool operator>=(const version& other) const;

            /*
             * @brief Compare two versions.
             */
            bool operator<=(const version& other) const;

            /*
             * @brief Compare two versions.
             */
            bool operator==(const version& other) const;
        };

        /**
         * @brief the current version of the libtrainsim
         *
         */
        const version lib_version(0, 12, 0);

        /**
         * @brief the current version of the json formats
         *
         */
        const version format_version(0, 9, 0);

    } // namespace core
} // namespace libtrainsim
