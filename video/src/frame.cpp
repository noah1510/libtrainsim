#include "frame.hpp"

using namespace libtrainsim;

Frame::Frame(){};

bool Frame::isEmpty() const {
    return true;
}

Frame::~Frame(){
    clear();
}

void Frame::clear(){
    return;
}
