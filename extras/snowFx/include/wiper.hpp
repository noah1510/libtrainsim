#pragma once

#include "imguiHandler.hpp"
#include "clampedVariable.hpp"
#include "helper.hpp"
#include "texture.hpp"
#include "shader.hpp"

namespace libtrainsim{
    namespace extras{
        class wiper{
        private:
            //all angles in degrees
            float minRotation = 7.5;
            float maxRotation = 167.5;
            libtrainsim::core::clampedVariable<float> currentRotation;
            float rotationPrecision = 0.25;
            float wiperSpeed = 0.5;
            bool turningLeft = true;
            
            const float wiperScaling = 0.85f;
            
            std::shared_ptr<libtrainsim::Video::texture> wiperImage;
            std::shared_ptr<libtrainsim::Video::texture> wiperFBO;
            std::shared_ptr<libtrainsim::Video::texture> wiperMask;
            
            std::shared_ptr<libtrainsim::Video::Shader> wiperShader;
            
            glm::mat4 coreTransform;
            
            glm::mat4 getWiperTransform() const;
            
            void wiperMaskClear();
            void updateWiperMask();
            
        public:
            wiper(const std::filesystem::path& shaderLocation, const std::filesystem::path& textureLocation);
            ~wiper();
            
            void setWiperSpeed(float newSpeed);
            void updateWiper(std::shared_ptr<libtrainsim::Video::texture> outputImage);
            void displayWiper(std::shared_ptr<libtrainsim::Video::texture> outputImage);
            
            float getRotation() const;
        };
    }
}
