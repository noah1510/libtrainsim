#pragma once

#include "imguiHandler.hpp"

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <tuple>
#include <vector>

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
        
        class texture{
        private:
            std::string name;
            unsigned int textureID = 0;
            
            dimensions imageSize = {0.0f,0.0f};
            
            std::shared_mutex acessMutex;
        public:
            texture();
            texture(const std::string& _name);
            texture(const std::filesystem::path& URI);
            ~texture();
            
            const dimensions& getSize() noexcept;
            unsigned int getTextureId() noexcept;
            const std::string& getName() noexcept;
            void updateImage(const std::vector<uint8_t>& data, const dimensions& newSize);
            void updateImage(const uint8_t* data, const dimensions& newSize);
            void bind();
            
            std::shared_mutex& getMutex() noexcept;
        };
    }
}
