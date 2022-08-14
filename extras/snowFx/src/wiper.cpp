#include "wiper.hpp"

libtrainsim::extras::wiper::wiper(const std::filesystem::path& shaderLocation, const std::filesystem::path& textureLocation){
    try{
        wiperImage = std::make_shared<libtrainsim::Video::texture>(textureLocation/"wiper.tif");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create wiper texture"));
    }
    
    try{
        wiperFBO = std::make_shared<libtrainsim::Video::texture>();
        wiperFBO->createFramebuffer({1920,1080});
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create wiper framebuffer"));
    }
    
    try{
        wiperShader = std::make_shared<libtrainsim::Video::Shader>(shaderLocation/"copy.vert",shaderLocation/"wiper.frag");
        wiperShader->use();
        wiperShader->setUniform("img",0);
        wiperShader->setUniform("mask",1);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create wiper shader"));
    }
    
}

libtrainsim::extras::wiper::~wiper() {
}

void libtrainsim::extras::wiper::updateWiper ( libtrainsim::Video::texture& outputImage ) {
    glm::mat4 transform{1.0f};
    transform = glm::rotate(transform, 0.1f, {0.0f,0.0f,1.0f});
    wiperFBO->loadFramebuffer();
    wiperShader->use();
    
}

