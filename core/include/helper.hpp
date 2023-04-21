#pragma once

#include "simplegfx.hpp"

#ifndef LIBTRAINSIM_EXPORT_MACRO
    #define LIBTRAINSIM_EXPORT_MACRO
#endif

namespace libtrainsim {
    namespace core {
        /**
         * @brief A Helper class for Functionality needed across all classes and components
         *
         */
        class LIBTRAINSIM_EXPORT_MACRO Helper : public SimpleGFX::helper {
          private:
            /**
             * @brief Construct a new Helper object
             *
             */
            Helper();

            /**
             * @brief get the instance of the helper
             * This function basically guarantees that there is only one instance of the helper
             *
             * @return Helper&
             */
            static Helper& get() {
                static Helper instance{};
                return instance;
            }

          public:
        };
    } // namespace core
} // namespace libtrainsim
