#include <catch2/catch.hpp>

#include <iostream>
#include "train_properties.hpp"

using namespace libtrainsim::core;


TEST_CASE( "Checking if load track fails with invalid inputs", "[vector]" ) {
    const auto dat1 = train_properties("");
    const auto dat2 = train_properties("meson.build");
    REQUIRE_FALSE(dat1.isValid());
    REQUIRE_FALSE(dat2.isValid());
};

TEST_CASE( "Checking if load track works with valid input", "[vector]" ) {
    const auto dat = train_properties("../core/tests/test_train_data_1.json");
    REQUIRE(dat.isValid());
};

