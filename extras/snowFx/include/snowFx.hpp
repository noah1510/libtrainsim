#pragma once

#include "imguiHandler.hpp"

#include "helper.hpp"
#include "texture.hpp"
#include "shader.hpp"

#include "speed.hpp"

#include <chrono>
#include <random>
#include <vector>
#include <memory>
#include <filesystem>

namespace libtrainsim{
    namespace extras{
        class snowFx{
          private:
            //all of the textures for the snowflake image
            std::vector< std::shared_ptr<libtrainsim::Video::texture> > snowflake_textures;
            
            //the shader for the opengl operations
            std::shared_ptr<libtrainsim::Video::Shader> blitShader;
            std::shared_ptr<libtrainsim::Video::Shader> copyShader;
            std::shared_ptr<libtrainsim::Video::Shader> blurShader;
            
            //the texture for the image output
            std::shared_ptr<libtrainsim::Video::texture> outputTexture;
            
            //the buffer and texture used to create the image output
            std::shared_ptr<libtrainsim::Video::texture> imageTexture;
            
            //the buffer and texture used to create the blurred output
            std::shared_ptr<libtrainsim::Video::texture> blurTexture;
            
            //all of the other buffers needed for the shaders
            unsigned int VBO = 0, VAO = 0, EBO = 0;
            
            //initialize random numbers
            std::mt19937_64 number_generator;
            
            //all the number distributions
            std::uniform_int_distribution<> distribution_image;
            std::uniform_real_distribution<> distribution_x;
            std::uniform_real_distribution<> distribution_y;
            std::uniform_real_distribution<> distribution_size;
            std::uniform_real_distribution<> distribution_rotation;
            std::uniform_real_distribution<> distribution_deltaT;
            std::uniform_real_distribution<> distribution_copyBlur;
            
            //a time difference since the last snowflake to indicate when the next should be shown
            decltype(std::chrono::microseconds(100000)) next_snowflake;
            //a time point when the last snowflake was drawn
            decltype(libtrainsim::core::Helper::now()) last_snowflake;
            
            //this loads a snowflake file into a texture buffer
            std::shared_ptr<libtrainsim::Video::texture> loadSnowflake(const std::filesystem::path& URI);
            
            //a simple multiplier to change the rate at which new snowflakes spawn
            double weather_intensity = 0.1;
            
            //draws a snowflake to the output buffer if needed
            void drawSnowflake();
            
            //copies the output buffer into the input buffer
            void copy(std::shared_ptr<libtrainsim::Video::texture> src, std::shared_ptr<libtrainsim::Video::texture> dest, bool loadTexture = true);
            
            //blur a texture with a given amount of passes
            void blur(std::shared_ptr<libtrainsim::Video::texture> tex, uint64_t passes = 4);
            
            //load a framebuffer with the wanted loader settigns
            void loadFramebuffer(std::shared_ptr<libtrainsim::Video::texture> buf);
            
            //The current speed of the train. This modifies in interval in which new snowflakes are spawned.
            sakurajin::unit_system::common::speed trainSpeed;
            
          public:
            snowFx(const std::filesystem::path& shaderLocation, const std::filesystem::path& dataLocation);
            
            ~snowFx();
            
            void updateTexture();
            
            std::shared_ptr<libtrainsim::Video::texture> getOutputTexture();
            
            void updateTrainSpeed(sakurajin::unit_system::common::speed newTrainSpeed);
            
        };
    }
}
