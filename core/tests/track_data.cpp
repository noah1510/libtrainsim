#include <gtest/gtest.h>

#include "libtrainsim_config.hpp"
#include "track_data.hpp"
#include <iostream>

using namespace sakurajin::unit_system::literals;

const static std::filesystem::path testPath = std::filesystem::path{TEST_DATA_DIRECTORY} / "core";

void test_nearest(const libtrainsim::core::Track_data& dat) {
    EXPECT_TRUE(dat.getSize() == 10);

    EXPECT_TRUE(dat.getFrame(0.04264325_m) == 8);
    EXPECT_TRUE(dat.getFrame(0.04264328_m) == 8);
    EXPECT_TRUE(dat.getFrame(0.02132164_m) == 5);
}


TEST(TrackData, getNearestFrame) {

    std::filesystem::path                        loc = testPath / "test_track_data.json";
    std::optional<libtrainsim::core::Track_data> dat;
    EXPECT_NO_THROW(dat = libtrainsim::core::Track_data{loc});

    test_nearest(dat.value());
}

TEST(TrackData, InValidCheck) {
    std::filesystem::path loc1 = "";
    std::filesystem::path loc2 = "meson.build";

    EXPECT_ANY_THROW(libtrainsim::core::Track_data{loc1});
    EXPECT_ANY_THROW(libtrainsim::core::Track_data{loc2});
}

TEST(TrackData, ValidCheck) {
    std::filesystem::path                        loc = testPath / "test_track_data.json";
    std::optional<libtrainsim::core::Track_data> dat;
    EXPECT_NO_THROW(dat = libtrainsim::core::Track_data{loc});

    EXPECT_TRUE(dat->getSize() == 10);
    EXPECT_TRUE(sakurajin::unit_system::unit_cast(dat->firstLocation(), 1).value == 0.0);
    EXPECT_TRUE(dat->lastLocation() == 0.04264326_m);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
