#include "texture.hpp"

libtrainsim::Video::dimensions::dimensions ( float x, float y ) noexcept : std::pair<float,float>{x,y}{}
libtrainsim::Video::dimensions::dimensions ( double x, double y ) noexcept : dimensions{static_cast<float>(x),static_cast<float>(y)}{}
libtrainsim::Video::dimensions::dimensions ( int x, int y ) noexcept : dimensions{static_cast<float>(x),static_cast<float>(y)}{}


float& libtrainsim::Video::dimensions::x() noexcept {
    return first;
}

float& libtrainsim::Video::dimensions::y()  noexcept{
    return second;
}

const float& libtrainsim::Video::dimensions::x() const noexcept{
    return first;
}

const float& libtrainsim::Video::dimensions::y() const noexcept{
    return second;
}

libtrainsim::Video::dimensions::operator ImVec2() const  noexcept{
     return ImVec2{first,second};
}

libtrainsim::Video::dimensions::operator glm::vec2 () const  noexcept{
    return glm::vec2{first,second};
}


libtrainsim::Video::texture::texture() {
    std::scoped_lock lock{acessMutex};
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);

    name = textureID;
}

libtrainsim::Video::texture::texture ( const std::string& _name ) : texture{} {
    std::scoped_lock lock{acessMutex};
    
    if(_name.empty() || _name.size() <= 1){
        throw std::invalid_argument("not a valid name for a texture:" + _name);
    }
    
    name = _name;
}


libtrainsim::Video::texture::~texture(){
    std::scoped_lock lock{acessMutex};
    
    glDeleteTextures(1, &textureID);
}

const libtrainsim::Video::dimensions & libtrainsim::Video::texture::getSize() noexcept {
    std::shared_lock lock{acessMutex};
    return imageSize;
}

unsigned int libtrainsim::Video::texture::getTextureId() noexcept {
    std::shared_lock lock{acessMutex};
    return textureID;
}

std::shared_mutex & libtrainsim::Video::texture::getMutex() noexcept {
    return acessMutex;
}

void libtrainsim::Video::texture::updateImage (const std::vector<uint8_t>& data, const libtrainsim::Video::dimensions& newSize ) {
    updateImage(data.data(),newSize);
}

void libtrainsim::Video::texture::updateImage (const uint8_t* data, const libtrainsim::Video::dimensions& newSize ) {
    imageSize = newSize;
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    auto [w,h] = imageSize;
    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_RGBA, 
        w, 
        h, 
        0,
        GL_RGBA, 
        GL_UNSIGNED_BYTE, 
        data
    );
}

