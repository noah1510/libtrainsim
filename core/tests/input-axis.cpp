#include <gtest/gtest.h>

#include "input_axis.hpp"

using namespace libtrainsim::core;

TEST(InputAxisTest, CheckCompare) {

    const input_axis ax1 = 2.0;
    EXPECT_TRUE(ax1.get() == 1.0);

    const input_axis ax2 = -2.0;
    EXPECT_TRUE(ax2.get() == -1.0);

    const input_axis ax3 = 0.0;
    EXPECT_TRUE((ax1 + ax2).get() == 0.0);
    EXPECT_TRUE((ax1 + ax2) == ax3);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
