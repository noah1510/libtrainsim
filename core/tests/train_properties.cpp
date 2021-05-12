#include <catch2/catch.hpp>

#include <iostream>
#include "train_properties.hpp"

using namespace libtrainsim::core;

using namespace sakurajin::unit_system::base::literals;
using namespace sakurajin::unit_system::common::literals;

TEST_CASE( "Checking if load track fails with invalid inputs", "[vector]" ) {
    const auto dat1 = train_properties("");
    const auto dat2 = train_properties("meson.build");
    const auto dat3 = train_properties("../core/tests/data/test_train_data_2.json");
    REQUIRE_FALSE(dat1.isValid());
    REQUIRE_FALSE(dat2.isValid());
    REQUIRE_FALSE(dat3.isValid());
};

TEST_CASE( "Checking if load track works with valid input", "[vector]" ) {
    const auto dat = train_properties("../core/tests/data/test_train_data_1.json");
    REQUIRE(dat.isValid());
    REQUIRE(dat.getName() == "DB-Baureihe 423/433");
    REQUIRE(dat.getMaxVelocity() == 140_kmph);
    REQUIRE(dat.getMaxAcceleration() == 1_mps2);
    REQUIRE(dat.getMass() == 119400_kg);
};

