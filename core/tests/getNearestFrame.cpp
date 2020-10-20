#include <catch2/catch.hpp>

#include "track_data.hpp"
#include <iostream>

void test_nearest_c(const libtrainsim::Track_data& dat){
    REQUIRE(dat.isValid());
    REQUIRE(dat.getSize() == 10);

    REQUIRE(dat.getFrame(0.04264325) == 7);
    REQUIRE(dat.getFrame(0.04264328) == 9);
    REQUIRE(dat.getFrame(0.02132164) == 7);
}

void test_nearest(libtrainsim::Track_data* dat){
    REQUIRE(dat->isValid());
    REQUIRE(dat->getSize() == 10);

    REQUIRE(dat->getFrame(0.04264325) == 7);
    REQUIRE(dat->getFrame(0.04264328) == 9);
    REQUIRE(dat->getFrame(0.02132164) == 7);
}

TEST_CASE( "Checking if getNearestFrame works", "[vector]" ) {
    
    auto dat = libtrainsim::Track_data("../core/tests/test_dat.json");
    const auto dat_c = libtrainsim::Track_data("../core/tests/test_dat.json");

    test_nearest_c(dat);
    test_nearest_c(dat_c);

    test_nearest(&dat);

};


