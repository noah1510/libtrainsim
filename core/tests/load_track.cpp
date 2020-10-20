#include <catch2/catch.hpp>

#include "track_data.hpp"

TEST_CASE( "Checking if load track fails with invalid inputs", "[vector]" ) {
    const auto dat1 = libtrainsim::Track_data("");
    const auto dat2 = libtrainsim::Track_data("meson.build");
    REQUIRE_FALSE(dat1.isValid());
    REQUIRE_FALSE(dat2.isValid());
};

TEST_CASE( "Checking if load track works with valid input", "[vector]" ) {
    const auto dat = libtrainsim::Track_data("../core/tests/test_dat.json");
    REQUIRE(dat.isValid());
};
