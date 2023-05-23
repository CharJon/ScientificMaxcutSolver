#include <gtest/gtest.h>

#include <cmath>

#include "sms/auxiliary/statistics.hpp"

TEST(getStatistics, IntTest) {
    std::vector<int> vals = {-5, 4, -3, 2, -1, 5, 1, -2, 3, -4, 0};

    auto stats = getStatistics(vals);

    ASSERT_EQ(stats.max, 5);
    ASSERT_EQ(stats.mean, 0);
    ASSERT_EQ(stats.min, -5);
    ASSERT_EQ(stats.median, 0);
    ASSERT_EQ(stats.standard_dev, std::sqrt(10));
}

TEST(getStatistics, UlongTest) {
    std::vector<ulong> vals = {5, 4, 3, 2, 1, 5, 1, 2, 3, 4};

    auto stats = getStatistics(vals);

    ASSERT_EQ(stats.max, 5);
    ASSERT_EQ(stats.mean, 3);
    ASSERT_EQ(stats.min, 1);
    ASSERT_EQ(stats.median, 3);
    ASSERT_EQ(stats.standard_dev, std::sqrt(2));
}

TEST(getStatistics, UlongTest2) {
    std::vector<ulong> vals = {5, 1, 1, 3};

    auto stats = getStatistics(vals);

    ASSERT_EQ(stats.max, 5);
    ASSERT_EQ(stats.mean, 2.5);
    ASSERT_EQ(stats.min, 1);
    ASSERT_EQ(stats.median, 1);
    ASSERT_EQ(stats.mode, 1);
    ASSERT_EQ(stats.standard_dev, std::sqrt(2.75));
}


TEST(getStatistics, DoubleTest) {
    std::vector<double> vals = {-.5, .4, -.3, .2, -.1, .1, -.2, .3, -.4, .5};

    auto stats = getStatistics(vals);

    ASSERT_EQ(stats.max, .5);
    ASSERT_EQ(stats.mean, 0);
    ASSERT_EQ(stats.min, -.5);
    ASSERT_EQ(stats.median, -.1);
    ASSERT_EQ(stats.standard_dev, 0.1 * std::sqrt(11));
}

TEST(getStatistics, ModeTest) {
    std::vector<int> vals = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 3, -1, -1, -1, -1};

    auto stats = getStatistics(vals);

    ASSERT_EQ(stats.max, 3);
    ASSERT_EQ(stats.mean, 0.75);
    ASSERT_EQ(stats.min, -1);
    ASSERT_EQ(stats.median, 1);
    ASSERT_EQ(stats.standard_dev, std::sqrt(1.5875));
    ASSERT_EQ(stats.mode, 1);
}