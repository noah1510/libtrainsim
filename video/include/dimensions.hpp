#pragma once

#include <tuple>


#include <glad/glad.h>
#include "imgui.h"
#include "glm/glm.hpp"

namespace libtrainsim{
    namespace Video{
        class dimensions : public std::pair<float, float>{
        private:
            
        public:
            dimensions(float x, float y) noexcept;
            dimensions(double x, double y) noexcept;
            dimensions(int x, int y) noexcept;
            
            float& x() noexcept;
            float& y() noexcept;
            
            const float& x() const noexcept;
            const float& y() const noexcept;
            
            operator glm::vec2() const noexcept;
            operator ImVec2() const noexcept;
        };
    }
}
