#pragma once

#include <tuple>


#include <glad/glad.h>
#include "imgui.h"
#include "glm/glm.hpp"
#include "helper.hpp"

namespace libtrainsim{
    namespace Video{
        class dimensions : public std::pair<float, float>{
        private:
            
        public:
            dimensions(float x, float y) noexcept;
            dimensions(double x, double y) noexcept;
            dimensions(int x, int y) noexcept;
            dimensions(const ImVec2& val) noexcept;
            
            float& x() noexcept;
            float& y() noexcept;
            
            const float& x() const noexcept;
            const float& y() const noexcept;
            
            operator glm::vec2() const noexcept;
            operator ImVec2() const noexcept;

            bool isRoughly(const dimensions& other) const noexcept;
        };
    }
}
