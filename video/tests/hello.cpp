#include <catch2/catch.hpp>

#include "video.hpp"

TEST_CASE( "Checking if hello method works", "[vector]" ) {
    const auto hello = libtrainsim::video::hello();
    REQUIRE(hello.compare("Hello from the video singleton") == 0);
};
