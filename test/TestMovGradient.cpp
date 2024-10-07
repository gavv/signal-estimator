// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/MovGradient.hpp"

#include <gtest/gtest.h>

using namespace signal_estimator;

namespace {

TEST(MovGradientTest, InitialState) {
    MovGradient<double> gradient;
    EXPECT_EQ(0.0, gradient.get());
}

TEST(MovGradientTest, SingleValue) {
    MovGradient<double> gradient;
    gradient.add(5.0);
    EXPECT_EQ(0.0, gradient.get());
}

TEST(MovGradientTest, TwoValues) {
    MovGradient<double> gradient;
    gradient.add(5.0);
    gradient.add(10.0);
    EXPECT_EQ(0, gradient.get());
}

TEST(MovGradientTest, ThreeValues) {
    MovGradient<double> gradient;
    gradient.add(5.0);
    gradient.add(10.0);
    gradient.add(15.0);
    EXPECT_EQ(5.0, gradient.get());
}

TEST(MovGradientTest, MoreValues) {
    MovGradient<double> gradient;
    gradient.add(5.0);
    gradient.add(10.0);
    gradient.add(15.0);
    gradient.add(20.0);
    // Calculate the expected gradient manually based on the central difference formula
    EXPECT_EQ((20.0 - 10.0) / 2, gradient.get());
}

} // namespace
