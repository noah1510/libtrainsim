#include "snowFx.hpp"

using namespace std::literals;

libtrainsim::extras::snowFx::snowFx(const std::filesystem::path& shaderLocation, const std::filesystem::path& dataLocation){
    
    //---------------load the shaders and textures---------------
    //shaders
    try{
        blitShader = std::make_shared<libtrainsim::Video::Shader>(shaderLocation/"blit.vert",shaderLocation/"blit.frag");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create blit shader"));
    }
    
    try{
        copyShader = std::make_shared<libtrainsim::Video::Shader>(shaderLocation/"copy.vert",shaderLocation/"copy.frag");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create copy shader"));
    }
    
    try{
        blurShader = std::make_shared<libtrainsim::Video::Shader>(shaderLocation/"blur.vert",shaderLocation/"blur.frag");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not create blur shader"));
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
    
    
    //---------------init vertex buffers---------------
    float vertices[] = {
        // position           // texture coords
         1.0f,  1.0f,   1.0f, 0.0f, // top right
         1.0f, -1.0f,   1.0f, 1.0f, // bottom right
        -1.0f, -1.0f,   0.0f, 1.0f, // bottom left
        -1.0f,  1.0f,   0.0f, 0.0f  // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    
    //create all of the blit buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    //---------------initialize the rng variables---------------
    std::random_device random_seed;
    number_generator = std::mt19937_64{random_seed()};
    
    distribution_image = std::uniform_int_distribution<>{0, static_cast<int>(snowflake_textures.size()-1)};
    distribution_x = std::uniform_real_distribution<>{-1.1, 1.0};
    distribution_y = std::uniform_real_distribution<>{-1.1, 1.0};
    distribution_size = std::uniform_real_distribution<>{0.01,0.025};
    distribution_rotation = std::uniform_real_distribution<> {0.0, 2*std::acos(0.0)};
    distribution_deltaT = std::uniform_real_distribution<> {0.5, 2.0};
    
    //the the ime when the lase snowflake was drawn and when the next will be drawn
    next_snowflake = 100000us * static_cast<long>( distribution_deltaT(number_generator) );
    last_snowflake = libtrainsim::core::Helper::now();
}

libtrainsim::extras::snowFx::~snowFx() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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
    copy(flake_tex, flake);
    blur(flake, 20);
    
    //return the framebuffer texture.
    return flake_tex;
}

void libtrainsim::extras::snowFx::blur ( std::shared_ptr<libtrainsim::Video::texture> tex, uint64_t passes ) {
    if(!tex->hasFramebuffer()){
        throw std::invalid_argument("the texture cannot be blured since it has no framebuffer to be rendered into.");
    }
    
    blurShader->use();
    
    glActiveTexture(GL_TEXTURE14);
    blurTexture->bind();
        
    glActiveTexture(GL_TEXTURE15);
    tex->bind();
    
    glBindVertexArray(VAO);
    
    auto resolution = glm::vec2{3840.0f, 2160.0f};
    glm::vec2 off1x = glm::vec2(1.3846153846) * glm::vec2(1.0,0.0) / resolution;
    glm::vec2 off1y = glm::vec2(1.3846153846) * glm::vec2(0.0,1.0) / resolution;
    glm::vec2 off2x = glm::vec2(3.2307692308) * glm::vec2(1.0,0.0) / resolution;
    glm::vec2 off2y = glm::vec2(3.2307692308) * glm::vec2(0.0,1.0) / resolution;
    
    for(uint64_t i = 0; i < passes/2; i++){
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
    }
}

void libtrainsim::extras::snowFx::copy ( std::shared_ptr<libtrainsim::Video::texture> src, std::shared_ptr<libtrainsim::Video::texture> dest, bool loadTexture ) {
    if(src == nullptr || dest == nullptr){
        throw std::invalid_argument("nullptr not allowed for copy operation");
    }
    
    if(!dest->hasFramebuffer()){
        throw std::invalid_argument("destination texture has no attached framebuffer");
    }
    loadFramebuffer(dest);
    
    copyShader->use();
    auto orth = glm::ortho(
        -1.0f, 
        1.0f,
        -1.0f,
        1.0f,
        -10.0f,
        10.0f
    );
    copyShader->setUniform("transform", orth);
    
    if(loadTexture){
        glActiveTexture(GL_TEXTURE15);
        src->bind();
        
        copyShader->setUniform("sourceImage", 15);
    }
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    //reset all of the buffers
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void libtrainsim::extras::snowFx::drawSnowflake() {
    auto dt = std::chrono::high_resolution_clock::now() - last_snowflake;
    if (dt > next_snowflake){
        glm::mat4 projection = glm::mat4(1.0f);
        
        //render the new snowflake into the output framebuffer
        loadFramebuffer(outputTexture);
        
        blitShader->use();
        
        //draw the background
        blitShader->setUniform("transform", projection);
        
        glActiveTexture(GL_TEXTURE0);
        imageTexture->bind();
        blitShader->setUniform("img", 0);
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        //draw the snowflake
        auto flake = snowflake_textures[ distribution_image(number_generator) ];
        flake->bind();
        
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
        
        blitShader->setUniform("transform", orth * projection);
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        //update when the time when snowflake was drawn and when the next one should be drawn
        last_snowflake = std::chrono::high_resolution_clock::now();
        next_snowflake = 100000us * static_cast<long>( distribution_deltaT(number_generator) / weather_intensity );
    }
}

void libtrainsim::extras::snowFx::updateTexture() {
    drawSnowflake();
    
    //copy the result back into the input framebuffer
    copy(outputTexture, imageTexture);
    
    blur(outputTexture, 6);
}

std::shared_ptr<libtrainsim::Video::texture> libtrainsim::extras::snowFx::getOutputTexture() {
    return outputTexture;
}


