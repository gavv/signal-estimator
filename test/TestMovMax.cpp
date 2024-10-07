// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/MovMax.hpp"

#include <gtest/gtest.h>

using namespace signal_estimator;

namespace {

// One obj value MovMax init test
TEST(MovMaxTest, EmptyMovMax) {
    MovMax<int> movmax(5);
    movmax.add(42);
    EXPECT_EQ(movmax.get(), 42);
}

// Test for large values
TEST(MovMaxTest, LargeMovMax) {
    MovMax<double> movmax(10);
    for (int i = 1; i <= 10; ++i) {
        EXPECT_EQ(movmax(i), i);
    }
}

// The test checks that MovMax correctly adds values and returns the maximum value among
// added values.
TEST(MovMaxTest, AddAndGetMax) {
    MovMax<double, false> movmax(5);

    movmax.add(5.0);
    movmax.add(10.0);
    movmax.add(3.0);

    ASSERT_EQ(movmax.get(), 10.0);
}

// The test checks that MovMax correctly adds values and returns the maximum value among
// added values with preload.
TEST(MovMaxTest, PreloadedAddAndGetMax) {
    MovMax<double, true> movmax(3);

    movmax.add(2.0);
    movmax.add(8.0);

    ASSERT_EQ(movmax.get(), 8.0);
}

// Different types test
TEST(MovMaxTest, MixedTypeMovMax) {
    MovMax<float> movmax(4);
    EXPECT_EQ(movmax(5.5f), 5.5f);
    EXPECT_EQ(movmax(2), 5.5f);
    EXPECT_EQ(movmax(8.5), 8.5);
    EXPECT_EQ(movmax(3.3f), 8.5);
}

} // namespace
