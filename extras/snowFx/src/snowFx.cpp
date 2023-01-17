#include "snowFx.hpp"

using namespace std::literals;
using namespace sakurajin::unit_system::literals;

libtrainsim::extras::snowFx::snowFx(std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf){
    
    auto snowLocation = conf->getExtrasLocation() / "snowFx";
    auto shaderLocation = snowLocation / "shaders";
    auto textureLocation = snowLocation / "textures";
    
    //---------------load the shaders and textures---------------
    try{
        displacementShader = std::make_shared<libtrainsim::Video::Shader>(shaderLocation/"displacement.vert",shaderLocation/"displacement.frag");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create displacement shader"));
    }
    
    //framebuffers
    FBOSize = libtrainsim::Video::imguiHandler::getDefaultFBOSize();
    try{
        outputTexture = std::make_shared<libtrainsim::Video::texture>();
        imageTexture = std::make_shared<libtrainsim::Video::texture>();
        blurTexture = std::make_shared<libtrainsim::Video::texture>();
        
        outputTexture->createFramebuffer(FBOSize);
        imageTexture->createFramebuffer(FBOSize);
        blurTexture->createFramebuffer(FBOSize);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create framebuffers"));
    }
    
    //textures
    try{
        //adding all of the snowflake textures        
        for(int i = 0; i < 16; i++){
            std::stringstream URI{};
            URI << "snowflake-" << i << ".tif";
            auto flake = loadSnowflake(textureLocation/URI.str());
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
            auto disTex = std::make_shared<libtrainsim::Video::texture>(textureLocation/URI.str());
            displacementTextures.emplace_back(disTex);
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not load snowflake textures"));
    }
    
    //init the wiper
    try{
        wiperHandler = std::make_shared<libtrainsim::extras::wiper>(conf);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create wiper"));
    }
    
    //---------------initialize the rng variables---------------
    std::random_device random_seed;
    number_generator = std::mt19937_64{random_seed()};
    
    distribution_image = std::uniform_int_distribution<>{0, static_cast<int>(snowflake_textures.size()-1)};
    distribution_x = std::uniform_real_distribution<>{-1.5, 1.5};
    distribution_y = std::uniform_real_distribution<>{-1.1, 1.0};
    distribution_size = std::uniform_real_distribution<>{0.01,0.025};
    distribution_rotation = std::uniform_real_distribution<> {0.0, 2*std::acos(0.0)};
    distribution_deltaT = std::uniform_real_distribution<> {0.5, 2.0};
    distribution_displacementStrength = std::uniform_real_distribution<> {1.25, 2.25};
    
    updateTrainSpeed(0.0_mps);
    
    //the the ime when the lase snowflake was drawn and when the next will be drawn
    updateDrawTimes();
}

libtrainsim::extras::snowFx::~snowFx() {
}

void libtrainsim::extras::snowFx::loadFramebuffer ( std::shared_ptr<libtrainsim::Video::texture> buf ) {
    glBindFramebuffer(GL_FRAMEBUFFER, buf->getFBO());
    glViewport(0, 0, FBOSize.x(), FBOSize.y());

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

decltype ( libtrainsim::core::Helper::now() ) libtrainsim::extras::snowFx::generateNewTime(const decltype(libtrainsim::core::Helper::now())& timestamp) {
    auto randVal = distribution_deltaT(number_generator);
    auto multiplier = randVal * randVal / (weather_intensity * speedModifier);
    auto deltaT = 100000us * multiplier;
    return timestamp + std::chrono::duration_cast<std::chrono::milliseconds>(deltaT);
}


void libtrainsim::extras::snowFx::updateDrawTimes() {
    if(nextSnowflakeDrawTimes.size() == 0){
        nextSnowflakeDrawTimes.emplace_back( generateNewTime( libtrainsim::core::Helper::now() ));
    }

    auto currTime = libtrainsim::core::Helper::now();
    
    //queue all snowflakes to be drawn in the next 0.5 seconds
    uint64_t arrSize = nextSnowflakeDrawTimes.size();
    while( currTime + 1.0s > nextSnowflakeDrawTimes.back() && ++arrSize <= maxSnowflakes){
        auto nextTime =  generateNewTime( nextSnowflakeDrawTimes.back() );
        nextSnowflakeDrawTimes.emplace_back(nextTime);
    }
}

bool libtrainsim::extras::snowFx::shouldDrawSnowflake(const decltype(libtrainsim::core::Helper::now())& timestamp) {
    if(nextSnowflakeDrawTimes.size() == 0){
        updateDrawTimes();
        return false;
    }

    if(timestamp > nextSnowflakeDrawTimes.front()){
        nextSnowflakeDrawTimes.erase(nextSnowflakeDrawTimes.begin());
        return true;
    }
    
    return false;
}


glm::mat4 libtrainsim::extras::snowFx::getSnowflakeTransformation() {
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
    constexpr float halfScreenWidth = (16.0f/9.0f + 1.0f)/2.0f;
    auto orth = glm::ortho(
        -halfScreenWidth, 
        halfScreenWidth,
        -1.0f,
        1.0f,
        -10.0f,
        10.0f
    );
    
    return orth * projection;
}

void libtrainsim::extras::snowFx::drawSnowflake() {
    
    //load the actual displacement map
    displacementTextures[1]->bind(1);
    
    //draw the snowflake
    auto flake = snowflake_textures[ distribution_image(number_generator) ];
    flake->bind(0);
    
    //set the displacement multiplier to a random number
    //the 150 is an arbitray value, it is supposed to be close to the maximum speed of the train
    //the faster the train is the more mashed up the snowflakes are
    float displacementStrength = distribution_displacementStrength(number_generator) * 0.1 * std::cbrt(speedModifier);
    displacementShader->setUniform("multiplier", displacementStrength);
    
    //set the transformation for the snowflake
    auto transform = getSnowflakeTransformation();
    displacementShader->setUniform("transform", transform);
    
    //actually draw the next snowflake
    libtrainsim::Video::imguiHandler::bindVAO();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void libtrainsim::extras::snowFx::updateTexture() {
    
    //copy the snowflake layer onto the output
    copyMoveDown(outputTexture, imageTexture);
    
    //draw new snowflak(s) if needed
    snowflakesDrawn = 0;
    auto currTime = libtrainsim::core::Helper::now();
    while(shouldDrawSnowflake(currTime)){
        //draw a new snowflake if needed
        drawSnowflake();
        snowflakesDrawn++;
    }
    
    //copy the output back into the input layer to keep snowflakes next draw
    copyMoveDown(imageTexture, outputTexture);
    
    //wipe where the wiper is
    wiperHandler->updateWiper(imageTexture);
    
    //display the wiped snowflake layer
    copyMoveDown(outputTexture, imageTexture);
    
    //display the wiper with its current position
    wiperHandler->displayWiper(outputTexture);
    
    //refill the timestamps for the next snowflakes to be generated
    updateDrawTimes();
    
}

void libtrainsim::extras::snowFx::updateTrainSpeed ( sakurajin::unit_system::speed newTrainSpeed ) {
    newTrainSpeed = sakurajin::unit_system::unit_cast(newTrainSpeed,1.0);
    auto& val = newTrainSpeed.value;
    val = val < 1 ? 1 : val;
    //this guarantees that log2(sqrt(val)) > 0
    //speedModifier = std::log2( newTrainSpeed.value  + 2) - 0.25;
    speedModifier = newTrainSpeed.value;
    wiperHandler->setWiperSpeed( std::sqrt( speedModifier ) );
}


std::shared_ptr<libtrainsim::Video::texture> libtrainsim::extras::snowFx::getOutputTexture() {
    return outputTexture;
}

uint64_t libtrainsim::extras::snowFx::getSnowflakeCount() const {
    return snowflakesDrawn;
}


std::shared_ptr<libtrainsim::extras::wiper> libtrainsim::extras::snowFx::getWiper() {
    return wiperHandler;
}



