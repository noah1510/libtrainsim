#include "dimensions.hpp"

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
