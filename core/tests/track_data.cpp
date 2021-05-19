#include <catch2/catch.hpp>

#include <iostream>
#include "track_data.hpp"

using namespace sakurajin::unit_system::base::literals;

void test_nearest_c(const libtrainsim::core::Track_data& dat){
    REQUIRE(dat.isValid());
    REQUIRE(dat.getSize() == 10);

    REQUIRE(dat.getFrame(0.04264325_m) == 8);
    REQUIRE(dat.getFrame(0.04264328_m) == 8);
    REQUIRE(dat.getFrame(0.02132164_m) == 5);
}

void test_nearest(libtrainsim::core::Track_data* dat){
    REQUIRE(dat->isValid());
    REQUIRE(dat->getSize() == 10);

    REQUIRE(dat->getFrame(0.04264325_m) == 8);
    REQUIRE(dat->getFrame(0.04264328_m) == 8);
    REQUIRE(dat->getFrame(0.02132164_m) == 4);
}

TEST_CASE( "Checking if getNearestFrame works", "[vector]" ) {
    
    libtrainsim::core::Track_data dat{"../core/tests/data/test_track_data.json"};
    const libtrainsim::core::Track_data dat_c{"../core/tests/data/test_track_data.json"};

    test_nearest_c(dat);
    test_nearest_c(dat_c);

    test_nearest(&dat);

};

TEST_CASE( "Checking if load track fails with invalid inputs", "[vector]" ) {
    const libtrainsim::core::Track_data dat1{""};
    const libtrainsim::core::Track_data dat2{"meson.build"};
    REQUIRE_FALSE(dat1.isValid());
    REQUIRE_FALSE(dat2.isValid());
};

TEST_CASE( "Checking if load track works with valid input", "[vector]" ) {
    const libtrainsim::core::Track_data dat{"../core/tests/data/test_track_data.json"};
    REQUIRE(dat.isValid());
    
    REQUIRE(dat.getSize() == 10);
    REQUIRE(sakurajin::unit_system::unit_cast(dat.firstLocation(),1).value == 0.0);
    REQUIRE(dat.lastLocation() == 0.04264326_m);
};
