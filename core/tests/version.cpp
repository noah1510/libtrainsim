using namespace libtrainsim::core;

TEST(VersionTests, CheckCompare) {
    auto v1 = version("0.4.0");
    auto v2 = version(0, 4, 0);
    auto v3 = version(1, 0, 0);
    auto v4 = version(0, 5, 0);
    auto v5 = version(0, 4, 1);

    EXPECT_EQ(version::compare(v1, v2), 0);

    EXPECT_TRUE(version::compare(v2, v3) < 0);
    EXPECT_TRUE(version::compare(v3, v2) > 0);

    EXPECT_TRUE(version::compare(v2, v4) < 0);
    EXPECT_TRUE(version::compare(v4, v2) > 0);

    EXPECT_TRUE(version::compare(v2, v5) < 0);
    EXPECT_TRUE(version::compare(v5, v2) > 0);

    EXPECT_EQ(v1, v2);

    EXPECT_TRUE(v2 < v3);
    EXPECT_TRUE(v3 > v2);

    EXPECT_TRUE(v2 < v4);
    EXPECT_TRUE(v4 > v2);

    EXPECT_TRUE(v2 < v5);
    EXPECT_TRUE(v5 > v2);

    auto [major, minor, patch] = v5.Version;
    EXPECT_EQ(major, 0);
    EXPECT_EQ(minor, 4);
    EXPECT_EQ(patch, 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
