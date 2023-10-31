#include <gtest/gtest.h>
#include "MovAvg.hpp"


TEST(MovAvgTest, InitialValue) {
    signal_estimator::MovAvg<double> movAvg(3);
    movAvg.add(1.0);
    EXPECT_DOUBLE_EQ(movAvg.get(), 1.0);
    
    movAvg.add(2.0);
    EXPECT_DOUBLE_EQ(movAvg.get(), 1.5);

    movAvg.add(3.0);
    EXPECT_DOUBLE_EQ(movAvg.get(), 2.0);

    EXPECT_TRUE(movAvg.is_full());

    movAvg.add(4.0);
    EXPECT_DOUBLE_EQ(movAvg.get(), 3.0);
    
    movAvg.add(5.0);
    EXPECT_DOUBLE_EQ(movAvg.get(), 4.0);
}

