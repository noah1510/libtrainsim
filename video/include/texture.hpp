#pragma once

#include "imguiHandler.hpp"

#include "dimensions.hpp"

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <tuple>
#include <vector>

namespace libtrainsim{
    namespace Video{

        class texture{
        private:
            std::string name;
            unsigned int textureID = 0;
            
            dimensions imageSize = {0.0f,0.0f};
            
            std::shared_mutex acessMutex;
            
            bool framebufferMode = false;
            
            unsigned int FBO = 0;
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
            void resize(const dimensions& newSize);
            
            void bind();
            void createFramebuffer(dimensions framebufferSize);
            void loadFramebuffer();
            void displayImGui();
            
            std::shared_mutex& getMutex() noexcept;
        };
    }
}
