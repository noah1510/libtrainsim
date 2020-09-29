#include <catch2/catch.hpp>

#include "format.hpp"

TEST_CASE( "Checking if load track fails with invalid inputs", "[vector]" ) {
    REQUIRE(libtrainsim::Format::loadTrack(""));
    REQUIRE(libtrainsim::Format::loadTrack("meson.build"));
};

TEST_CASE( "Checking if load track works with valid input", "[vector]" ) {
    REQUIRE_FALSE(libtrainsim::Format::loadTrack("../core/tests/test_dat.json"));
};
