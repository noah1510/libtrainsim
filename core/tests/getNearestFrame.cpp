#include <catch2/catch.hpp>

#include "format.hpp"
#include <iostream>

TEST_CASE( "Checking if getNearestFrame works", "[vector]" ) {
    REQUIRE_FALSE(libtrainsim::Format::loadTrack("../core/tests/test_dat.json"));
    REQUIRE(libtrainsim::Format::getSize() == 10);

    REQUIRE(libtrainsim::Format::getFrame(0.04264325) == 7);
    std::cout << "passed first test" << std::endl;

    REQUIRE(libtrainsim::Format::getFrame(0.04264328) == 9);
    std::cout << "passed second test" << std::endl;

    REQUIRE(libtrainsim::Format::getFrame(0.02132164) == 7);
    std::cout << "passed third test" << std::endl;

};
