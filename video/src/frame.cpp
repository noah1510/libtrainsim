#include "frame.hpp"

using namespace libtrainsim;

Frame::Frame(){};

VideoBackends Frame::getBackend() const{
    return currentBackend;
}
