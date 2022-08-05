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

libtrainsim::Video::texture::texture ( const std::filesystem::path& URI ) :texture{} {
   
    if( !std::filesystem::exists( URI) ){
        throw std::runtime_error(std::string{"image file does not exist! "}.append(URI));
    }
    
    std::scoped_lock<std::shared_mutex>{acessMutex};
    auto tmp_surface = IMG_Load(URI.c_str());
    if(!tmp_surface) {
        throw std::runtime_error(std::string{"Could not read image: "}.append(IMG_GetError()));
    }
    
    auto* surface = SDL_ConvertSurfaceFormat(tmp_surface, SDL_PIXELFORMAT_RGBA32, 0);
    if(surface == nullptr){
        SDL_FreeSurface(tmp_surface);
        throw std::runtime_error(std::string{"Could not convert to rgba32: "}.append(SDL_GetError()));
    }
    
    SDL_FreeSurface(tmp_surface);
    
    SDL_SetSurfaceRLE(surface, true);
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
    
    // set texture filtering parameters
    glBindTexture(GL_TEXTURE_2D ,textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    auto w = surface->w;
    auto h = surface->h;
    
    //upload the texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        w,
        h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        surface->pixels
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    SDL_FreeSurface(surface);
    
    imageSize.x() = w;
    imageSize.y() = h;
    
    name = URI;
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

