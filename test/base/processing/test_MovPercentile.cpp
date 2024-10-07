// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/MovPercentile.hpp"

#include <gtest/gtest.h>

using namespace signal_estimator;

namespace {

TEST(MovPercentileTest, AddSingleValue) {
    MovPercentile<double> percentile(5, 0.5);
    percentile.add(5.0);
    EXPECT_FALSE(percentile.is_full());
    EXPECT_EQ(5.0, percentile.get());
}

TEST(MovPercentileTest, AddMultipleValues) {
    MovPercentile<double> percentile(5, 0.5);
    percentile.add(5.0);
    percentile.add(10.0);
    percentile.add(15.0);
    percentile.add(20.0);
    percentile.add(25.0);

    EXPECT_TRUE(percentile.is_full());
    EXPECT_EQ(15.0, percentile.get());
}

TEST(MovPercentileTest, AddValuesBeyondWindow) {
    MovPercentile<double> percentile(5, 0.5);
    percentile.add(5.0);
    percentile.add(10.0);
    percentile.add(15.0);
    percentile.add(20.0);
    percentile.add(25.0);
    percentile.add(30.0);
    percentile.add(35.0);

    EXPECT_TRUE(percentile.is_full());
    EXPECT_EQ(25.0, percentile.get());
}

TEST(MovPercentileTest, AddValuesInDescendingOrder) {
    MovPercentile<double> percentile(5, 0.5);
    percentile.add(30.0);
    percentile.add(25.0);
    percentile.add(20.0);
    percentile.add(15.0);
    percentile.add(10.0);
    percentile.add(5.0);

    EXPECT_TRUE(percentile.is_full());
    EXPECT_EQ(15, percentile.get());
}

TEST(MovPercentileTest, AddValuesInAscendingOrder) {
    MovPercentile<double> percentile(5, 0.5);
    percentile.add(5.0);
    percentile.add(10.0);
    percentile.add(15.0);
    percentile.add(20.0);
    percentile.add(25.0);
    percentile.add(30.0);

    EXPECT_TRUE(percentile.is_full());
    EXPECT_EQ(20.0, percentile.get());
}

} // namespace
