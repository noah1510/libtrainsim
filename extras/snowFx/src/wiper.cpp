#include "wiper.hpp"

libtrainsim::extras::wiper::wiper(const std::filesystem::path& textureLocation){
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
    
}

libtrainsim::extras::wiper::~wiper() {
}

void libtrainsim::extras::wiper::updateWiper ( libtrainsim::Video::texture& outputImage ) {
    
}

