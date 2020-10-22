#include <catch2/catch.hpp>

#include <iostream>
#include "track_data.hpp"

void test_nearest_c(const libtrainsim::core::Track_data& dat){
    REQUIRE(dat.isValid());
    REQUIRE(dat.getSize() == 10);

    REQUIRE(dat.getFrame(0.04264325) == 7);
    REQUIRE(dat.getFrame(0.04264328) == 9);
    REQUIRE(dat.getFrame(0.02132164) == 7);
}

void test_nearest(libtrainsim::core::Track_data* dat){
    REQUIRE(dat->isValid());
    REQUIRE(dat->getSize() == 10);

    REQUIRE(dat->getFrame(0.04264325) == 7);
    REQUIRE(dat->getFrame(0.04264328) == 9);
    REQUIRE(dat->getFrame(0.02132164) == 7);
}

TEST_CASE( "Checking if getNearestFrame works", "[vector]" ) {
    
    auto dat = libtrainsim::core::Track_data("../core/tests/data/test_track_data.json");
    const auto dat_c = libtrainsim::core::Track_data("../core/tests/data/test_track_data.json");

    test_nearest_c(dat);
    test_nearest_c(dat_c);

    test_nearest(&dat);

};

TEST_CASE( "Checking if load track fails with invalid inputs", "[vector]" ) {
    const auto dat1 = libtrainsim::core::Track_data("");
    const auto dat2 = libtrainsim::core::Track_data("meson.build");
    REQUIRE_FALSE(dat1.isValid());
    REQUIRE_FALSE(dat2.isValid());
};

TEST_CASE( "Checking if load track works with valid input", "[vector]" ) {
    const auto dat = libtrainsim::core::Track_data("../core/tests/data/test_track_data.json");
    REQUIRE(dat.isValid());
    
    REQUIRE(dat.getSize() == 10);
    REQUIRE(dat.firstLocation() == 0.0);
    REQUIRE(dat.lastLocation() == 0.04264326);
};
