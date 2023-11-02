#pragma once

#include "imguiHandler.hpp"

#include "helper.hpp"
#include "subprojects/simplegl/include/gl/defaultShader.hpp"
#include "subprojects/simplegl/include/gl/shader.hpp"
#include "subprojects/simplegl/include/gl/texture.hpp"

#include "wiper.hpp"

#include "unit_system.hpp"

#include <chrono>
#include <random>
#include <vector>
#include <memory>
#include <filesystem>

namespace libtrainsim{
    namespace extras{
        class LIBTRAINSIM_EXPORT_MACRO snowFx{
          private:
            //all of the textures for the snowflake image
            std::vector< std::shared_ptr<SimpleGFX::SimpleGL::texture> > snowflake_textures;
            
            //the shader for the opengl operations
            std::shared_ptr<SimpleGFX::SimpleGL::Shader> displacementShader;
            
            //the texture for the image output
            std::shared_ptr<SimpleGFX::SimpleGL::texture> outputTexture;
            
            //the buffer and texture used to create the image output
            std::shared_ptr<SimpleGFX::SimpleGL::texture> imageTexture;
            
            //all of the displacement textures for various types of effects
            std::vector< std::shared_ptr<SimpleGFX::SimpleGL::texture> > displacementTextures;
            
            //initialize random numbers
            std::mt19937_64 number_generator;
            
            //all the number distributions
            std::uniform_int_distribution<> distribution_image;
            std::uniform_real_distribution<> distribution_x;
            std::uniform_real_distribution<> distribution_y;
            std::uniform_real_distribution<> distribution_size;
            std::uniform_real_distribution<> distribution_rotation;
            std::uniform_real_distribution<> distribution_deltaT;
            std::uniform_real_distribution<> distribution_displacementStrength;
            
            //The time stamps at which the next snowflake should be drawn
            std::vector< decltype(libtrainsim::core::Helper::now()) > nextSnowflakeDrawTimes;
            decltype(libtrainsim::core::Helper::now()) generateNewTime(const decltype(libtrainsim::core::Helper::now())& timestamp);
            
            //this loads a snowflake file into a texture buffer
            std::shared_ptr<SimpleGFX::SimpleGL::texture> loadSnowflake(const std::filesystem::path& URI);
            
            //a simple multiplier to change the rate at which new snowflakes spawn
            double weather_intensity = 1.0;
            const uint64_t maxSnowflakes = 20;
            double speedModifier = 1.1;
            
            //draws a snowflake to the output buffer if needed
            void drawSnowflakes();
            
            //a version of the copy shader that also slightly moves the image down
            void copyMoveDown(std::shared_ptr<SimpleGFX::SimpleGL::texture> dest, std::shared_ptr<SimpleGFX::SimpleGL::texture> source);
            
            //load a framebuffer with the wanted loader settigns
            void loadFramebuffer(std::shared_ptr<SimpleGFX::SimpleGL::texture> buf);
            
            //the object to handle all of the wiper stuff
            std::shared_ptr<wiper> wiperHandler;
            
            glm::mat4 getSnowflakeTransformation();
            
            bool shouldDrawSnowflake(const decltype(libtrainsim::core::Helper::now())& timestamp);
            void updateDrawTimes();
            
            SimpleGFX::SimpleGL::dimensions FBOSize = {0,0};
            
          public:
            snowFx(std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf);
            
            ~snowFx();
            
            void updateTexture();
            
            std::shared_ptr<SimpleGFX::SimpleGL::texture> getOutputTexture();
            
            void updateTrainSpeed(sakurajin::unit_system::speed newTrainSpeed);
            
            std::shared_ptr<wiper> getWiper();
            
        };
    }
}
