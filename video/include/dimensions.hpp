#pragma once

#include <tuple>


#include <glad/glad.h>
#include "imgui.h"
#include "glm/glm.hpp"
#include "helper.hpp"

namespace libtrainsim{
    namespace Video{
        /**
         * @brief a small class to handle dimensions
         */
        class dimensions : public std::pair<float, float>{
          private:
          public:
            /**
             * @brief create a new dimensions object from two floats
             */
            dimensions(float x, float y) noexcept;
            
            /**
             * @brief create a new dimensions object from two doubles
             */
            dimensions(double x, double y) noexcept;
            
            /**
             * @brief create a new dimensions object from two ints
             */
            dimensions(int x, int y) noexcept;
            /**
             * @brief create a new dimensions object from an imgui vector
             */
            dimensions(const ImVec2& val) noexcept;
            
            /**
             * @brief get access to the x part
             */
            float& x() noexcept;
            
            /**
             * @brief get access to the y part
             */
            float& y() noexcept;
            
            /**
             * @brief get access to the x part (read only)
             */
            const float& x() const noexcept;
            
            /**
             * @brief get access to the y part (read only)
             */
            const float& y() const noexcept;
            
            /**
             * @brief convert the dimensions to an glm::vec2
             */
            operator glm::vec2() const noexcept;
            
            /**
             * @brief convert the dimensions to an ImGui vector
             */
            operator ImVec2() const noexcept;

            /**
             * @brief check if two dimensions are roughly the same
             */
            bool isRoughly(const dimensions& other) const noexcept;
        };
    }
}
