#pragma once

#include "imguiHandler.hpp"
#include "clampedVariable.hpp"
#include "helper.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "simulator_config.hpp"

namespace libtrainsim{
    namespace extras{
        class wiper{
        private:
            //The rotation angle minimum in degrees
            float minRotation = 7.5;
            
            //the rotation angle maximum in degrees
            float maxRotation = 167.5;
            
            //The current rotation in degrees
            libtrainsim::core::clampedVariable<float> currentRotation;
            
            /**
             * @brief The rotation Precision in degrees
             * This controls how fine the movement of the wiper is allowed to be.
             * Smaller values cause less stripes between the movement steps of the wiper.
             * 
             */
            float rotationPrecision = 0.25;
            
            //The current speed of the wiper. The angle will be changed by this amount each frame.
            float wiperSpeed = 0.5;
            
            //indicate that the wiper will be move to the left when true.
            bool turningLeft = true;
            
            //The size scaling of the wiper 
            const float wiperScaling = 0.85f;
            
            //The loaded wiper texture
            std::shared_ptr<libtrainsim::Video::texture> wiperImage;
            
            //an output buffer for the wiper
            std::shared_ptr<libtrainsim::Video::texture> wiperFBO;
            
            //The mask that is applied for actually wiping off an texture
            std::shared_ptr<libtrainsim::Video::texture> wiperMask;
            
            //The shader to wipe snow off a surface
            std::shared_ptr<libtrainsim::Video::Shader> wiperShader;
            
            //the constant transformations to speed up the calculations
            glm::mat4 coreTransform;
            
            //create the transormation matrix with the current rotation
            glm::mat4 getWiperTransform() const;
            
            //clear the wiper buffer to init the next wipe
            void wiperMaskClear();
            
            //add another wiper image step to the wiper mask
            void updateWiperMask();
            
        public:
            /**
             * @brief create a new wiper
             * 
             */
            wiper(std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf);
            
            /**
             * @brief destroy the wiper object
             */
            ~wiper();
            
            /**
             * @brief set the speed of the wiper
             */
            void setWiperSpeed(float newSpeed);
            
            /**
             * @brief update the wiper mask and wipe parts from a given image
             * 
             */
            void updateWiper(std::shared_ptr<libtrainsim::Video::texture> outputImage);
            
            /**
             * @brief display the wiper texture on an image
             * 
             */
            void displayWiper(std::shared_ptr<libtrainsim::Video::texture> outputImage);
            
            /**
             * @brief get the current rotation in degrees
             * 
             */
            float getRotation() const;
        };
    }
}
