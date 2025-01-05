#include "core/exceptionUtils.hpp"
#include "core/numberUtils.hpp"
using namespace sakurajin::unit_system::literals;

const static std::filesystem::path testPath = std::filesystem::path{TEST_DATA_DIRECTORY} / "core";

std::unique_ptr<libtrainsim::core::Track> get_test_track() {
    auto LOGGER = std::make_shared<SimpleGFX::core::logger>(SimpleGFX::core::loggingLevel::debug);

    std::filesystem::path                     loc = testPath / "test_track_data.json";
    std::unique_ptr<libtrainsim::core::Track> dat = nullptr;

    try {
        dat = std::make_unique<libtrainsim::core::Track>(LOGGER, loc, false);
    } catch (const std::exception& e) {

        std::stringstream error_stream;
        SimpleGFX::core::decodeException(e, error_stream);

        ADD_FAILURE() << "Could not load track from " << loc.string() << "\nGot the following error:\n " << error_stream.str();
        return nullptr;
    }

    dat->ensure();

    return dat;
}


void test_nearest(const libtrainsim::core::Track& dat) {
    // EXPECT_TRUE(dat.getSize() == 10);
}

TEST(TrackData, getNearestFrame) {
    auto data = get_test_track();
    EXPECT_NE(data, nullptr);

    EXPECT_EQ(data->getFrame(0.04_m), 0);
    EXPECT_EQ(data->getFrame(0.07_m), 1);
    EXPECT_EQ(data->getFrame(0.21_m), 2);
    EXPECT_EQ(data->getFrame(0.33_m), 3);
    EXPECT_EQ(data->getFrame(0.39_m), 4);
    EXPECT_EQ(data->getFrame(0.50_m), 5);
    EXPECT_EQ(data->getFrame(0.56_m), 6);
    EXPECT_EQ(data->getFrame(0.74_m), 7);
    EXPECT_EQ(data->getFrame(0.83_m), 8);
    EXPECT_EQ(data->getFrame(1.00_m), 9);
}

TEST(TrackData, InValidCheck) {
    auto                                      LOGGER = std::make_shared<SimpleGFX::core::logger>(SimpleGFX::core::loggingLevel::debug);
    std::filesystem::path                     loc1   = "";
    std::filesystem::path                     loc2   = "meson.build";
    std::unique_ptr<libtrainsim::core::Track> dat    = nullptr;

    try {
        dat = std::make_unique<libtrainsim::core::Track>(LOGGER, loc1, false);
        ADD_FAILURE() << "No exception was triggered!";
    } catch (...) {
    }

    try {
        dat = std::make_unique<libtrainsim::core::Track>(LOGGER, loc2, false);
        ADD_FAILURE() << "No exception was triggered!";
    } catch (...) {
    }
}

TEST(TrackData, ValidCheck) {
    auto data = get_test_track();
    EXPECT_NE(data, nullptr);

    // EXPECT_TRUE(dat->getSize() == 10);
    EXPECT_DOUBLE_EQ(sakurajin::unit_system::unit_cast(data->firstLocation(), 1).val(), 0.0);

    auto last_loc_val = data->lastLocation().convert_like(1_m).val();
    EXPECT_DOUBLE_EQ(last_loc_val, 0.9);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
