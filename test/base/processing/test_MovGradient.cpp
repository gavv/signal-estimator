#include <gtest/gtest.h>
#include "MovGradient.hpp"

using namespace signal_estimator;

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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}