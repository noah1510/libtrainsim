#include <gtest/gtest.h>

#include <iostream>
#include "train_properties.hpp"

using namespace libtrainsim::core;

using namespace sakurajin::unit_system::literals;

TEST(Helper, isRoughly){
    EXPECT_FALSE(Helper::isRoughly<double>(1.0,-1.0));
    EXPECT_TRUE(Helper::isRoughly<int>(1001,1000));
};

int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
