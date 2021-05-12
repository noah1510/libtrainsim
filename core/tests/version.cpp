#include <catch2/catch.hpp>

#include "types.hpp"

using namespace libtrainsim::core;

TEST_CASE( "Checking if version compare works", "[vector]" ) {
    auto v1 = version("0.4.0");
    auto v2 = version(0,4,0);
    auto v3 = version(1,0,0);
    auto v4 = version(0,5,0);
    auto v5 = version(0,4,1);
    
    REQUIRE(version::compare(v1,v2) == 0);
    
    REQUIRE(version::compare(v2,v3) < 0);
    REQUIRE(version::compare(v3,v2) > 0);
    
    REQUIRE(version::compare(v2,v4) < 0);
    REQUIRE(version::compare(v4,v2) > 0);
    
    REQUIRE(version::compare(v2,v5) < 0);
    REQUIRE(version::compare(v5,v2) > 0);
    
    REQUIRE(v1 == v2);
    
    REQUIRE(v2 < v3);
    REQUIRE(v3 > v2);
    
    REQUIRE(v2 < v4);
    REQUIRE(v4 > v2);
    
    REQUIRE(v2 < v5);
    REQUIRE(v5 > v2);
    
    auto [major,minor,patch] = v5.Version;
    REQUIRE(major == 0);
    REQUIRE(minor == 4);
    REQUIRE(patch == 1);
}
