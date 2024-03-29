#include "wiper.hpp"

libtrainsim::extras::wiper::wiper(std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf):currentRotation(minRotation,maxRotation,0.0){
    
    auto wiperLocation = conf->getExtrasLocation() / "wiper";
    auto shaderLocation = wiperLocation / "shaders";
    auto textureLocation = wiperLocation / "textures";
    
    try{
        wiperImage = std::make_shared<SimpleGFX::SimpleGL::texture>(textureLocation/"wiper.tif");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create wiper texture"));
    }
    
    try{
        wiperFBO = std::make_shared<SimpleGFX::SimpleGL::texture>();
        wiperFBO->createFramebuffer(SimpleGFX::SimpleGL::imguiHandler::getDefaultFBOSize());
        wiperFBO->setClearColor({0,0,0,0});
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create wiper framebuffer"));
    }
    
    try{
        wiperMask = std::make_shared<SimpleGFX::SimpleGL::texture>();
        wiperMask->createFramebuffer(SimpleGFX::SimpleGL::imguiHandler::getDefaultFBOSize());
        wiperMask->setClearColor({0,0,0,0});
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create wiper mask framebuffer"));
    }
    
    try{
        wiperShader = std::make_shared<SimpleGFX::SimpleGL::Shader>(shaderLocation/"copy.vert",shaderLocation/"wiper.frag");
        wiperShader->use();
        wiperShader->setUniform("img",0);
        wiperShader->setUniform("mask",1);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create wiper shader."));
    }
    
    coreTransform = glm::mat4{1.0f};
    
    coreTransform = glm::translate(coreTransform, {wiperScaling/2,0.0,0.0});
    coreTransform = glm::scale(coreTransform, {wiperScaling,wiperScaling,1.0f});
}

libtrainsim::extras::wiper::~wiper() {
    
}

void libtrainsim::extras::wiper::setWiperSpeed ( float newSpeed ) {
    wiperSpeed = newSpeed;
}

glm::mat4 libtrainsim::extras::wiper::getWiperTransform() const {
    glm::mat4 transform{1.0f};
    glm::vec3 offset{wiperScaling/2,-wiperScaling,0.0};
    
    transform = glm::translate(transform, {wiperScaling/2.0f,wiperScaling/4.0f,0.0f});
    
    transform = glm::translate(transform, -offset);
    transform = glm::rotate(transform, -glm::radians(currentRotation.get()), {0.0f,0.0f,1.0f});
    transform = glm::translate(transform, offset);
    
    constexpr float halfScreenWidth = 16.0f/9.0f;
    auto orth = glm::ortho(
        -halfScreenWidth, 
        halfScreenWidth,
        -1.0f,
        1.0f,
        -10.0f,
        10.0f
    );
    
    return orth*transform*coreTransform;
}

void libtrainsim::extras::wiper::wiperMaskClear() {
    wiperMask->loadFramebuffer();
    auto shader = SimpleGFX::SimpleGL::imguiHandler::getCopyShader();
    shader->use();
    shader->setUniform("sourceImage",0);
}

void libtrainsim::extras::wiper::updateWiperMask() {
    auto shader = SimpleGFX::SimpleGL::imguiHandler::getCopyShader();
    
    wiperImage->bind(0);
    auto transform = getWiperTransform();
    shader->setUniform("transform", transform);
    SimpleGFX::SimpleGL::imguiHandler::drawRect();
    
}


void libtrainsim::extras::wiper::updateWiper ( std::shared_ptr<SimpleGFX::SimpleGL::texture> outputImage ) {
    //--update the angle of the wiper--
    libtrainsim::core::clampedVariable<float> nextRot = currentRotation;
    
    //clear the wiper mask before updating it
    wiperMaskClear();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if(turningLeft){
        nextRot += wiperSpeed;
        
        while(currentRotation < nextRot){
            currentRotation += rotationPrecision;
            updateWiperMask();
        }
        
        if(currentRotation.isRoughly(maxRotation)){
            turningLeft = false;
        }
        //turningLeft = !libtrainsim::core::Helper::isRoughly<float>(nextRot, maxRotation);
    }else{
        nextRot -= wiperSpeed;
        
        while(currentRotation > nextRot){
            currentRotation -= rotationPrecision;
            updateWiperMask();
        }
        
        if(currentRotation.isRoughly(minRotation)){
            turningLeft = true;
        }
        //turningLeft = libtrainsim::core::Helper::isRoughly<float>(nextRot, minRotation);
    }
    
    //--render the wiper image onto the mask--
    
    glDisable(GL_BLEND);
    
    //--wiper buffer--
    wiperFBO->loadFramebuffer();
    
    //activate the wiper shader
    wiperShader->use();
    
    //set the transformation matrix for a 1:1 copy
    //this assumes that all images are 16:9
    auto mat = glm::mat4(1.0f);
    wiperShader->setUniform("transform", mat);
    
    //bind the background and mask
    outputImage->bind(0);
    wiperMask->bind(1);
    
    //draw the mask onto the snow
    SimpleGFX::SimpleGL::imguiHandler::drawRect();
    
    //--copy the results back into the output image--
    SimpleGFX::SimpleGL::imguiHandler::copy(wiperFBO, outputImage);
    
}

void libtrainsim::extras::wiper::displayWiper ( std::shared_ptr<SimpleGFX::SimpleGL::texture> outputImage ) {
    SimpleGFX::SimpleGL::imguiHandler::copy(outputImage, wiperFBO);

    outputImage->loadFramebuffer();
    
    auto shader = SimpleGFX::SimpleGL::imguiHandler::getCopyShader();
    
    shader->use();
    wiperFBO->bind(0);
    shader->setUniform("sourceImage",0);
    SimpleGFX::SimpleGL::imguiHandler::drawRect();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    wiperImage->bind(0);
    auto transform = getWiperTransform();
    shader->setUniform("transform", transform);
    SimpleGFX::SimpleGL::imguiHandler::drawRect();
    glDisable(GL_BLEND);
    
}

float libtrainsim::extras::wiper::getRotation() const {
    return currentRotation.get();
}

