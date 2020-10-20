#include <catch2/catch.hpp>

#include "track_data.hpp"

TEST_CASE( "Checking if load track fails with invalid inputs", "[vector]" ) {
    const auto dat1 = libtrainsim::core::Track_data("");
    const auto dat2 = libtrainsim::core::Track_data("meson.build");
    REQUIRE_FALSE(dat1.isValid());
    REQUIRE_FALSE(dat2.isValid());
};

TEST_CASE( "Checking if load track works with valid input", "[vector]" ) {
    const auto dat = libtrainsim::core::Track_data("../core/tests/test_track_data.json");
    REQUIRE(dat.isValid());
};
