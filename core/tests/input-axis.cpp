#include <catch2/catch.hpp>

#include "input_axis.hpp"

using namespace libtrainsim::core;

TEST_CASE( "Checking if the input-axis behaves correctly works", "[vector]" ) {

    const input_axis ax1 = 2.0;
    REQUIRE(ax1.get() == 1.0);

    const input_axis ax2 = -2.0;
    REQUIRE(ax2.get() == -1.0);

    const input_axis ax3 = 0.0;
    REQUIRE( (ax1+ax2).get() == 0.0 );
    REQUIRE( (ax1+ax2) == ax3 );
    
}