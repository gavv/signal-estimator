#include <gtest/gtest.h>
#include "MovDev.hpp"

// Trying to get the average and maximum deviation with an empty window
TEST(MovDevTest, EmptyWindow) {
    signal_estimator::MovDev<double> movdev(5);
    
   
    EXPECT_DOUBLE_EQ(movdev.avg(), 0);
    EXPECT_DOUBLE_EQ(movdev.max(), 0);
}

// The test verifies that the MovDev class correctly calculates the mean and maximum deviation from the mean.
TEST(MovDevTest, AvgAndMax) {
    signal_estimator::MovDev<int> movdev(3);

    movdev.add(4);
    movdev.add(6);
    movdev.add(7);

    EXPECT_EQ(movdev.avg(), 1);
    EXPECT_EQ(movdev.max(), 2);
}

/*The test verifies that the MovDev object correctly calculates the mean and maximum 
absolute deviation in the case where the window is equal to the buffer size.*/
TEST(MovDevTest, FullWindow) {
    const size_t window_sz = 3;
    signal_estimator::MovDev<double> movdev(window_sz);

    movdev.add(1);
    movdev.add(2);
    movdev.add(3);

    EXPECT_DOUBLE_EQ(movdev.avg(), 0.66666666666666663);
    EXPECT_DOUBLE_EQ(movdev.max(), 1);
}

//Еhe test checks that the MovDev object correctly handles the case where no values have been added to it and returns the expected values of 0
TEST(MovDevTest, GetAvgAndMaxWhenEmpty) {
    const size_t window_sz = 3;
    signal_estimator::MovDev<double> movdev(window_sz);

    EXPECT_DOUBLE_EQ(movdev.avg(), 0);
    EXPECT_DOUBLE_EQ(movdev.max(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}