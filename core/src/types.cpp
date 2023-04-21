#include "types.hpp"

libtrainsim::core::version::version(uint64_t x, uint64_t y, uint64_t z)
    : Version{std::tuple<uint64_t, uint64_t, uint64_t>{x, y, z}} {}

libtrainsim::core::version::version(std::tuple<uint64_t, uint64_t, uint64_t> ver)
    : Version{ver} {}

libtrainsim::core::version::version(const std::string& ver)
    : version(std::stoi(Helper::splitString(ver, '.').at(0)),
              std::stoi(Helper::splitString(ver, '.').at(1)),
              std::stoi(Helper::splitString(ver, '.').at(2))){}

std::string libtrainsim::core::version::print() const {
    return std::to_string(major()) + "." + std::to_string(minor()) + "." + std::to_string(patch());
}

uint64_t libtrainsim::core::version::major() const {
    return std::get<0>(Version);
}

uint64_t libtrainsim::core::version::minor() const {
    return std::get<1>(Version);
}

uint64_t libtrainsim::core::version::patch() const {
    return std::get<2>(Version);
}

bool libtrainsim::core::version::operator>(const version& other) const {
    return this->Version > other.Version;
}

bool libtrainsim::core::version::operator<(const version& other) const {
    return this->Version < other.Version;
}

bool libtrainsim::core::version::operator>=(const version& other) const {
    return this->Version >= other.Version;
}

bool libtrainsim::core::version::operator<=(const version& other) const {
    return this->Version <= other.Version;
}

bool libtrainsim::core::version::operator==(const version& other) const {
    return this->Version == other.Version;
}
