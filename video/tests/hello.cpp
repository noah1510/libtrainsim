#include <catch2/catch.hpp>

#include "video.hpp"

TEST_CASE( "Checking if hello method works", "[vector]" ) {
    REQUIRE(libtrainsim::video::hello().compare("Hello from the video singleton") == 0);
};
