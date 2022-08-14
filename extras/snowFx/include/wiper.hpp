#pragma once

#include "imguiHandler.hpp"
#include "helper.hpp"
#include "texture.hpp"

namespace libtrainsim{
    namespace extras{
        class wiper{
        private:
            float minRotation = 0.0;
            float maxRotation = glm::pi<float>();
            float currentRotation = 0.0;
            bool turningLeft = true;
            
            std::shared_ptr<libtrainsim::Video::texture> wiperImage;
            std::shared_ptr<libtrainsim::Video::texture> wiperFBO;
            
        public:
            wiper(const std::filesystem::path& textureLocation);
            ~wiper();
            
            void updateWiper(libtrainsim::Video::texture& outputImage);
        };
    }
}
