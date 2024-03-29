#include "snowFx.hpp"

using namespace std::literals;
using namespace sakurajin::unit_system::literals;

libtrainsim::extras::snowFx::snowFx(std::shared_ptr<libtrainsim::core::simulatorConfiguration> conf){
    
    auto snowLocation = conf->getExtrasLocation() / "snowFx";
    auto textureLocation = snowLocation / "textures";
    
    //---------------load the shaders and textures---------------
    try{
        SimpleGFX::SimpleGL::Shader_configuration disp_config{
            SimpleGFX::SimpleGL::defaultShaderSources::getBasicVertexSource(),
            SimpleGFX::SimpleGL::defaultShaderSources::getDisplacementFragmentSource()
        };
        displacementShader = std::make_shared<SimpleGFX::SimpleGL::Shader>(disp_config);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create displacement shader"));
    }
    
    //framebuffers
    FBOSize = SimpleGFX::SimpleGL::imguiHandler::getDefaultFBOSize();
    try{
        outputTexture = std::make_shared<SimpleGFX::SimpleGL::texture>();
        imageTexture = std::make_shared<SimpleGFX::SimpleGL::texture>();
        
        outputTexture->createFramebuffer(FBOSize);
        imageTexture->createFramebuffer(FBOSize);

        SimpleGFX::SimpleGL::imguiHandler::copy(SimpleGFX::SimpleGL::imguiHandler::getDarkenTexture(0), outputTexture);
        SimpleGFX::SimpleGL::imguiHandler::copy(SimpleGFX::SimpleGL::imguiHandler::getDarkenTexture(0), imageTexture);
        
        outputTexture->setClearColor({0.0,0.0,0.0,0.0});
        imageTexture->setClearColor({0.0,0.0,0.0,0.0});
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
            auto disTex = std::make_shared<SimpleGFX::SimpleGL::texture>(textureLocation/URI.str());
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

void libtrainsim::extras::snowFx::loadFramebuffer ( std::shared_ptr<SimpleGFX::SimpleGL::texture> buf ) {
    buf->loadFramebuffer();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_MAX);
}


std::shared_ptr<SimpleGFX::SimpleGL::texture> libtrainsim::extras::snowFx::loadSnowflake ( const std::filesystem::path& URI ) {
    //load the texture and create a framebuffer to copy the tex into
    auto flake_tex = std::make_shared<SimpleGFX::SimpleGL::texture>(URI);
    //auto flake = std::make_shared<SimpleGFX::SimpleGL::texture>();
    //flake->createFramebuffer(flake_tex->getSize());
    
    //copy the texture into the framebuffer
    //SimpleGFX::SimpleGL::imguiHandler::copy(flake_tex, flake);
    
    //return the framebuffer texture.
    return flake_tex;
}

void libtrainsim::extras::snowFx::copyMoveDown(std::shared_ptr<SimpleGFX::SimpleGL::texture> dest, std::shared_ptr<SimpleGFX::SimpleGL::texture> source) {
    glm::mat4 projection = glm::mat4(1.0f);
    dest->loadFramebuffer();
    
    displacementShader->use();
    
    //draw the background
    displacementShader->setUniform("transform", projection);
    
    //load the fx layer as background
    source->bind(0);
    displacementShader->setUniform("img", 0);
    
    //load move down displacment texture
    displacementTextures[2]->bind(1);
    displacementShader->setUniform("displacement", 1);
    
    //set the displacement to 0, to not move the fx layer
    displacementShader->setUniform("multiplier", 0.003f);
    
    SimpleGFX::SimpleGL::imguiHandler::drawRect();
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

void libtrainsim::extras::snowFx::drawSnowflakes() {
    
    auto currTime = libtrainsim::core::Helper::now();

    //load the buffer and the shader
    //loadFramebuffer(outputTexture);
    outputTexture->loadFramebuffer();
    
    displacementShader->use();
    displacementShader->setUniform("img", 0);
    displacementShader->setUniform("displacement", 1);

    //load the actual displacement map
    displacementTextures[0]->bind(1);

    imageTexture->bind(0);
    displacementShader->setUniform("multiplier", 0.0f);
    SimpleGFX::SimpleGL::imguiHandler::drawRect();

    //activate the correct alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_MAX);
    
    //load the actual displacement map
    displacementTextures[1]->bind(1);

    float speedScaler = 0.1 * std::cbrt(speedModifier);

    //draw a new snowflake if needed
    while(shouldDrawSnowflake(currTime)){

        //draw the snowflake
        auto flake = snowflake_textures[ distribution_image(number_generator) ];
        flake->bind(0);
        
        //set the displacement multiplier to a random number
        //the 150 is an arbitray value, it is supposed to be close to the maximum speed of the train
        //the faster the train is the more mashed up the snowflakes are
        float displacementStrength = distribution_displacementStrength(number_generator) * speedScaler;
        displacementShader->setUniform("multiplier", displacementStrength);
        
        //set the transformation for the snowflake
        auto transform = getSnowflakeTransformation();
        displacementShader->setUniform("transform", transform);
        
        //actually draw the next snowflake
        SimpleGFX::SimpleGL::imguiHandler::drawRect();
    }
    
    glBlendFunc(GL_ONE, GL_ZERO);
    glBlendEquation(GL_FUNC_ADD);
    glDisable(GL_BLEND);
}


void libtrainsim::extras::snowFx::updateTexture() {
    
    //copy the snowflake layer onto the output
    //copyMoveDown(outputTexture, imageTexture);
    //SimpleGFX::SimpleGL::imguiHandler::copy(imageTexture, outputTexture);
    
    //draw new snowflak(s) if needed
    drawSnowflakes();
    
    //copy the output back into the input layer to keep snowflakes next draw
    //copyMoveDown(imageTexture, outputTexture);
    //SimpleGFX::SimpleGL::imguiHandler::copy(outputTexture, imageTexture);
    
    //wipe where the wiper is
    wiperHandler->updateWiper(outputTexture);
    
    //display the wiped snowflake layer
    copyMoveDown(imageTexture, outputTexture);
    //SimpleGFX::SimpleGL::imguiHandler::copy(imageTexture, outputTexture);
    
    //display the wiper with its current position
    wiperHandler->displayWiper(outputTexture);
    
    //refill the timestamps for the next snowflakes to be generated
    updateDrawTimes();

    SimpleGFX::SimpleGL::imguiHandler::unsetBuffers();
    
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


std::shared_ptr<SimpleGFX::SimpleGL::texture> libtrainsim::extras::snowFx::getOutputTexture() {
    return outputTexture;
}


std::shared_ptr<libtrainsim::extras::wiper> libtrainsim::extras::snowFx::getWiper() {
    return wiperHandler;
}



