#include <gtest/gtest.h>

#include <iostream>
#include "train_properties.hpp"
#include "libtrainsim_config.hpp"

using namespace libtrainsim::core;

using namespace sakurajin::unit_system::base::literals;
using namespace sakurajin::unit_system::common::literals;

const static std::filesystem::path testPath = std::filesystem::path{TEST_DATA_DIRECTORY} / "core";

TEST(TrainProperties, InValidCheck){
    EXPECT_ANY_THROW(train_properties{std::filesystem::path{""}});
    EXPECT_ANY_THROW(train_properties{std::filesystem::path{"meson.build"}});
    EXPECT_ANY_THROW(train_properties{testPath / "test_train_data_2.json"});
};

TEST(TrainProperties, ValidCheck){
    std::optional<libtrainsim::core::train_properties> dat;
    std::filesystem::path location = testPath/"test_train_data_1.json";
    EXPECT_NO_THROW(dat = libtrainsim::core::train_properties{location});
    EXPECT_TRUE(dat->getName() == "DB-Baureihe 423/433");
    EXPECT_TRUE(dat->getMaxPower() == 2350000_W);
    EXPECT_TRUE(dat->getMass() == 119400_kg);
};

int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
