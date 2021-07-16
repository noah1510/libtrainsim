#include <catch2/catch.hpp>

#include <iostream>
#include "train_properties.hpp"

using namespace libtrainsim::core;

using namespace sakurajin::unit_system::base::literals;
using namespace sakurajin::unit_system::common::literals;

TEST_CASE( "Checking if load track fails with invalid inputs", "[vector]" ) {
    const train_properties dat1{""};
    const train_properties dat2{"meson.build"};
    const train_properties dat3{"../core/tests/data/test_train_data_2.json"};
    REQUIRE_FALSE(dat1.isValid());
    REQUIRE_FALSE(dat2.isValid());
    REQUIRE_FALSE(dat3.isValid());
};

TEST_CASE( "Checking if load track works with valid input", "[vector]" ) {
    const libtrainsim::core::train_properties dat{"../core/tests/data/test_train_data_1.json"};
    REQUIRE(dat.isValid());
    REQUIRE(dat.getName() == "DB-Baureihe 423/433");
    REQUIRE(dat.getMaxPower() == 2350000_W);
    REQUIRE(dat.getMass() == 119400_kg);
};
