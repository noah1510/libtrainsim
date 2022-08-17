#include "wiper.hpp"

libtrainsim::extras::wiper::wiper(const std::filesystem::path& shaderLocation, const std::filesystem::path& textureLocation){
    try{
        wiperImage = std::make_shared<libtrainsim::Video::texture>(textureLocation/"wiper.tif");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create wiper texture"));
    }
    
    try{
        wiperFBO = std::make_shared<libtrainsim::Video::texture>();
        wiperFBO->createFramebuffer({3840,2160});
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create wiper framebuffer"));
    }
    
    try{
        wiperShader = std::make_shared<libtrainsim::Video::Shader>(shaderLocation/"copy.vert",shaderLocation/"wiper.frag");
        wiperShader->use();
        wiperShader->setUniform("img",0);
        wiperShader->setUniform("mask",1);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create wiper shader."));
    }
    
}

libtrainsim::extras::wiper::~wiper() {
}

void libtrainsim::extras::wiper::updateWiper ( std::shared_ptr<libtrainsim::Video::texture> outputImage ) {
    //update the angle of the wiper
    if(turningLeft){
        currentRotation += .5;
        if(currentRotation > maxRotation){
            currentRotation = maxRotation;
            turningLeft = false;
        }
    }else{
        currentRotation -= .5;
        if(currentRotation < minRotation){
            currentRotation = minRotation;
            turningLeft = true;
        }
    }
    
    //display the current output image in the background
    //libtrainsim::Video::imguiHandler::copy(outputImage, wiperFBO);
    
    //display the wiper
    wiperFBO->loadFramebuffer();
    /*
    glBindFramebuffer(GL_FRAMEBUFFER, wiperFBO->getFBO());
    glViewport(0, 0, 3840, 2160);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
        
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
    glBlendEquation(GL_FUNC_ADD);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    */
    
    //get the shader from the imgui handler
    //auto shader = libtrainsim::Video::imguiHandler::getCopyShader();
    auto shader = wiperShader;
    shader->use();
    //shader->setUniform("sourceImage", 0);
    
    outputImage->bind(0);
    auto mat = glm::mat4(1.0f);
    shader->setUniform("transform", mat);
    libtrainsim::Video::imguiHandler::drawRect();
    
    //create the base transformation matrix
    
    glm::mat4 transform{1.0f};
    glm::vec3 offset{0.5,0.5,0.0};
    
    //transform = glm::translate(transform, {0.5f,-0.50f,0.0f});
    
    //transform = glm::translate(transform, -offset);
    transform = glm::rotate(transform, -glm::radians(currentRotation), {0.0f,0.0f,1.0f});
    //transform = glm::rotate(transform, glm::radians(0.0f), {0.0f,0.0f,1.0f});
    //transform = glm::translate(transform, offset);
    transform = glm::scale(transform, {0.5f,0.5f,1.0f});
    
    //bind the wiper image and set the transformation
    wiperImage->bind(1);
    
    /*
    auto orth = glm::ortho(
        -1.0f, 
        1.0f,
        -1.0f,
        1.0f,
        -10.0f,
        10.0f
    );
    orth = wiperFBO->getProjection();
    */
    transform = glm::inverse(transform);
    shader->setUniform("maskTransform", transform);
    
    //draw the wiper
    libtrainsim::Video::imguiHandler::drawRect();
    
    //copy the results back into the output image
    libtrainsim::Video::imguiHandler::copy(wiperFBO, outputImage);
    
}

