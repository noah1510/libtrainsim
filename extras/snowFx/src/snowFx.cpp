#include "snowFx.hpp"

using namespace std::literals;
using namespace sakurajin::unit_system::common::literals;

libtrainsim::extras::snowFx::snowFx(const std::filesystem::path& shaderLocation, const std::filesystem::path& dataLocation){
    
    //load the shaders for imgui handler in case they are not loaded yet
    try{
        libtrainsim::Video::imguiHandler::loadShaders(shaderLocation);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not init imguiHandler opengl parts"));
    }
    
    //---------------load the shaders and textures---------------
    try{
        displacementShader = std::make_shared<libtrainsim::Video::Shader>(shaderLocation/"displacement.vert",shaderLocation/"displacement.frag");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create displacement shader"));
    }
    
    //framebuffers
    try{
        outputTexture = std::make_shared<libtrainsim::Video::texture>();
        imageTexture = std::make_shared<libtrainsim::Video::texture>();
        blurTexture = std::make_shared<libtrainsim::Video::texture>();
        
        outputTexture->createFramebuffer({3840,2160});
        imageTexture->createFramebuffer({3840,2160});
        blurTexture->createFramebuffer({3840,2160});
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create framebuffers"));
    }
    
    //textures
    try{
        //adding all of the snowflake textures        
        for(int i = 0; i < 16; i++){
            std::stringstream URI{};
            URI << "snowflake-" << i << ".tif";
            auto flake = loadSnowflake(dataLocation/URI.str());
            snowflake_textures.emplace_back(flake);
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not load snowflake textures"));
    }
    
    try{
        //adding all of the displacement textures        
        for(int i = 0; i < 3; i++){
            std::stringstream URI{};
            URI << "displacement-" << i << ".tif";
            auto disTex = std::make_shared<libtrainsim::Video::texture>(dataLocation/URI.str());
            displacementTextures.emplace_back(disTex);
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not load snowflake textures"));
    }
    
    //init the wiper
    try{
        wiperHandler = std::make_shared<libtrainsim::extras::wiper>(shaderLocation, dataLocation);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create wiper"));
    }
    
    //---------------initialize the rng variables---------------
    std::random_device random_seed;
    number_generator = std::mt19937_64{random_seed()};
    
    distribution_image = std::uniform_int_distribution<>{0, static_cast<int>(snowflake_textures.size()-1)};
    distribution_x = std::uniform_real_distribution<>{-1.1, 1.0};
    distribution_y = std::uniform_real_distribution<>{-1.1, 1.0};
    distribution_size = std::uniform_real_distribution<>{0.01,0.025};
    distribution_rotation = std::uniform_real_distribution<> {0.0, 2*std::acos(0.0)};
    distribution_deltaT = std::uniform_real_distribution<> {0.5, 2.0};
    distribution_displacementStrength = std::uniform_real_distribution<> {1.25, 2.25};
    
    updateTrainSpeed(0.0_mps);
    
    //the the ime when the lase snowflake was drawn and when the next will be drawn
    next_snowflake = 100000us * static_cast<long>( distribution_deltaT(number_generator) );
    last_snowflake = libtrainsim::core::Helper::now();
}

libtrainsim::extras::snowFx::~snowFx() {
}

void libtrainsim::extras::snowFx::loadFramebuffer ( std::shared_ptr<libtrainsim::Video::texture> buf ) {
    glBindFramebuffer(GL_FRAMEBUFFER, buf->getFBO());
    glViewport(0, 0, 3840, 2160);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
        
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_MAX);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


std::shared_ptr<libtrainsim::Video::texture> libtrainsim::extras::snowFx::loadSnowflake ( const std::filesystem::path& URI ) {
    //load the texture and create a framebuffer to copy the tex into
    auto flake_tex = std::make_shared<libtrainsim::Video::texture>(URI);
    auto flake = std::make_shared<libtrainsim::Video::texture>();
    flake->createFramebuffer(flake_tex->getSize());
    
    //copy the texture into the framebuffer
    libtrainsim::Video::imguiHandler::copy(flake_tex, flake);
    
    //return the framebuffer texture.
    return flake_tex;
}

void libtrainsim::extras::snowFx::copyMoveDown(std::shared_ptr<libtrainsim::Video::texture> output, std::shared_ptr<libtrainsim::Video::texture> input) {
    glm::mat4 projection = glm::mat4(1.0f);
    loadFramebuffer(output);
    
    displacementShader->use();
    
    //draw the background
    displacementShader->setUniform("transform", projection);
    
    //load the fx layer as background
    input->bind(0);
    displacementShader->setUniform("img", 0);
    
    //load move down displacment texture
    displacementTextures[2]->bind(1);
    displacementShader->setUniform("displacement", 1);
    
    //set the displacement to 0, to not move the fx layer
    displacementShader->setUniform("multiplier", 0.003f);
    
    libtrainsim::Video::imguiHandler::bindVAO();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void libtrainsim::extras::snowFx::drawSnowflake() {
    copyMoveDown(outputTexture, imageTexture);
    
    //load the actual displacement map
    displacementTextures[1]->bind(1);
    
    //draw the snowflake
    auto flake = snowflake_textures[ distribution_image(number_generator) ];
    flake->bind(0);
    
    //set the displacement multiplier to a random number
    //the 150 is an arbitray value, it is supposed to be close to the maximum speed of the train
    //the faster the train is the more mashed up the snowflakes are
    float displacementStrength = distribution_displacementStrength(number_generator) * 0.1 * std::log2(trainSpeed.value);
    displacementShader->setUniform("multiplier", displacementStrength);
    
    //create the transformation matrix for the new snowflake
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::translate(
        projection, 
        glm::vec3(
            distribution_x(number_generator), 
            distribution_y(number_generator),
            0.0
        )
    );
    
    projection = glm::rotate(
        projection, 
        static_cast<float>( distribution_rotation(number_generator) ), 
        glm::vec3(0.0, 0.0, 1.0)
    );
    
    float size = distribution_size(number_generator);
    projection = glm::scale(
        projection, 
        glm::vec3(size, size, 1.0)
    );
    
    //create the projection matrix for to handle some buffer issues
    float camMult = 1/16.0;
    auto [w,h] = outputTexture->getSize();
    auto orth = glm::ortho(
        -1.0f, 
        camMult * 9.0f * w / h,
        -1.0f,
        camMult * 16.0f * h / w,
        -10.0f,
        10.0f
    );
    
    displacementShader->setUniform("transform", orth * projection);
    
    libtrainsim::Video::imguiHandler::bindVAO();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    //update when the time when snowflake was drawn and when the next one should be drawn
    last_snowflake = std::chrono::high_resolution_clock::now();
    auto multiplier = distribution_deltaT(number_generator) / (weather_intensity * std::log2(trainSpeed.value));
    next_snowflake = 1000ms * static_cast<long>( multiplier );

    //copy the result back into the input framebuffer
    libtrainsim::Video::imguiHandler::copy(outputTexture, imageTexture);
}

void libtrainsim::extras::snowFx::updateTexture() {
    
    auto dt = std::chrono::high_resolution_clock::now() - last_snowflake;
    if (dt > next_snowflake){
        //draw a new snowflake if needed
        drawSnowflake();
    }else{
        //if no new snowflake has to be drawn just draw the previous fx layer
        copyMoveDown(outputTexture, imageTexture);
    }
    
    wiperHandler->updateWiper(imageTexture);
    wiperHandler->displayWiper(outputTexture);
    
}

void libtrainsim::extras::snowFx::updateTrainSpeed ( sakurajin::unit_system::common::speed newTrainSpeed ) {
    newTrainSpeed = sakurajin::unit_system::unit_cast(newTrainSpeed,1.0);
    auto& val = newTrainSpeed.value;
    //this guarantees that log2(val) > 0
    val = val < 1.5 ? 1.5 : val;
    trainSpeed = newTrainSpeed;
    wiperHandler->setWiperSpeed(std::log2(trainSpeed.value-1.0));
}


std::shared_ptr<libtrainsim::Video::texture> libtrainsim::extras::snowFx::getOutputTexture() {
    return outputTexture;
}

std::shared_ptr<libtrainsim::extras::wiper> libtrainsim::extras::snowFx::getWiper() {
    return wiperHandler;
}

