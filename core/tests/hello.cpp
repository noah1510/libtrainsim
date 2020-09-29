#include <catch2/catch.hpp>

#include "format.hpp"

TEST_CASE( "Checking if hello method works", "[vector]" ) {
    REQUIRE(libtrainsim::Format::hello().compare("Hello from the Format Singleton!") == 0);
};
