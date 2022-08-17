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
        blurShader = std::make_shared<libtrainsim::Video::Shader>(shaderLocation/"blur.vert",shaderLocation/"blur.frag");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create blur shader"));
    }
    
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
            //blur(flake);
            snowflake_textures.emplace_back(flake);
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not load snowflake textures"));
    }
    
    try{
        //adding all of the displacement textures        
        for(int i = 0; i < 2; i++){
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
    distribution_copyBlur = std::uniform_real_distribution<> {0.0, 100.0};
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
    
    //copy the texture into the framebuffer and then blur the framebuffer
    libtrainsim::Video::imguiHandler::copy(flake_tex, flake);
    blur(flake, 20);
    
    //return the framebuffer texture.
    return flake_tex;
}

void libtrainsim::extras::snowFx::blur ( std::shared_ptr<libtrainsim::Video::texture> tex, uint64_t passes ) {
    if(!tex->hasFramebuffer()){
        throw std::invalid_argument("the texture cannot be blured since it has no framebuffer to be rendered into.");
    }
    
    blurShader->use();
    
    blurTexture->bind(14);
    tex->bind(15);
    
    libtrainsim::Video::imguiHandler::bindVAO();
    
    auto resolution = glm::vec2{3840.0f, 2160.0f};
    glm::vec2 off1x = glm::vec2(1.3846153846) * glm::vec2(1.0,0.0) / resolution;
    glm::vec2 off1y = glm::vec2(1.3846153846) * glm::vec2(0.0,1.0) / resolution;
    glm::vec2 off2x = glm::vec2(3.2307692308) * glm::vec2(1.0,0.0) / resolution;
    glm::vec2 off2y = glm::vec2(3.2307692308) * glm::vec2(0.0,1.0) / resolution;
    
    uint64_t i = 0;
    do{
        //blur into blur fbo
        loadFramebuffer(blurTexture);
        
        blurShader->setUniform("sourceImage", 15);
        blurShader->setUniform("off1", off1x);
        blurShader->setUniform("off2", off2x);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        //blur into output fbo
        loadFramebuffer(tex);
        
        blurShader->setUniform("sourceImage", 14);
        blurShader->setUniform("off1", off1y);
        blurShader->setUniform("off2", off2y);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        i++;
    }while(i < passes/2);
}

void libtrainsim::extras::snowFx::drawSnowflake() {
    glm::mat4 projection = glm::mat4(1.0f);
    
    //render the new snowflake into the output framebuffer
    loadFramebuffer(outputTexture);
    
    displacementShader->use();
    
    //draw the background
    displacementShader->setUniform("transform", projection);
    
    //load the fx layer as background
    imageTexture->bind(0);
    displacementShader->setUniform("img", 0);
    
    //load the displacement texture
    displacementTextures[1]->bind(1);
    displacementShader->setUniform("displacement", 1);
    
    //set the displacement to 0, to not move the fx layer
    displacementShader->setUniform("multiplier", 0.0f);
    
    libtrainsim::Video::imguiHandler::bindVAO();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    //draw the snowflake
    auto flake = snowflake_textures[ distribution_image(number_generator) ];
    flake->bind(0);
    
    //set the displacement multiplier to a random number
    //the 150 is an arbitray value, it is supposed to be close to the maximum speed of the train
    //the faster the train is the more mashed up the snowflakes are
    float displacementStrength = distribution_displacementStrength(number_generator) * 0.1 * std::log2(trainSpeed.value);
    displacementShader->setUniform("multiplier", displacementStrength);
    
    //create the transformation matrix for the new snowflake
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
        libtrainsim::Video::imguiHandler::copy(outputTexture, imageTexture);
    }
    
    if(distribution_copyBlur(number_generator) < 0.01){
        blur(imageTexture, 2);
    }
    
    wiperHandler->updateWiper(outputTexture);
    
}

void libtrainsim::extras::snowFx::updateTrainSpeed ( sakurajin::unit_system::common::speed newTrainSpeed ) {
    newTrainSpeed = sakurajin::unit_system::unit_cast(newTrainSpeed,1.0);
    auto& val = newTrainSpeed.value;
    //this guarantees that log2(val) > 0
    val = val < 1.5 ? 1.5 : val;
    trainSpeed = newTrainSpeed;
}


std::shared_ptr<libtrainsim::Video::texture> libtrainsim::extras::snowFx::getOutputTexture() {
    return outputTexture;
}

