#include <catch2/catch.hpp>

#include "types.hpp"

using namespace libtrainsim::core;

#define cmp(x,y) version::compare(x,y)

TEST_CASE( "Checking if version compare works", "[vector]" ) {
    auto v1 = version("0.4.0");
    auto v2 = version(0,4,0);
    auto v3 = version(1,0,0);
    auto v4 = version(0,5,0);
    auto v5 = version(0,4,1);
    
    REQUIRE(cmp(v1,v2) == 0);
    
    REQUIRE(cmp(v2,v3) < 0);
    REQUIRE(cmp(v3,v2) > 0);
    
    REQUIRE(cmp(v2,v4) < 0);
    REQUIRE(cmp(v4,v2) > 0);
    
    REQUIRE(cmp(v2,v5) < 0);
    REQUIRE(cmp(v5,v2) > 0);
}
