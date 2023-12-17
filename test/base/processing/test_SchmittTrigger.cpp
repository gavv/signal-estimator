#include <gtest/gtest.h>
#include "SchmittTrigger.hpp"

using namespace signal_estimator;

TEST(SchmittTriggerTest, InitialState) {
    SchmittTrigger<double> schmittTrigger(0.5);
    EXPECT_FALSE(schmittTrigger.get());
}

TEST(SchmittTriggerTest, TriggerOnRisingEdge) {
    SchmittTrigger<double> schmittTrigger(0.5);

    EXPECT_FALSE(schmittTrigger(0.2));
    EXPECT_FALSE(schmittTrigger.get());

    EXPECT_FALSE(schmittTrigger(0.6));
    EXPECT_FALSE(schmittTrigger.get());

    EXPECT_FALSE(schmittTrigger(0.7));
    EXPECT_FALSE(schmittTrigger.get());
}

TEST(SchmittTriggerTest, NoTriggerOnStayingAboveUpperThreshold) {
    SchmittTrigger<double> schmittTrigger(0.5);

    EXPECT_FALSE(schmittTrigger(0.2));
    EXPECT_FALSE(schmittTrigger.get());

    EXPECT_FALSE(schmittTrigger(0.6));
    EXPECT_FALSE(schmittTrigger.get());

    EXPECT_FALSE(schmittTrigger(0.7));
    EXPECT_FALSE(schmittTrigger.get());
}

TEST(SchmittTriggerTest, TriggerOnFallingEdge) {
    SchmittTrigger<double> schmittTrigger(0.5);

    EXPECT_FALSE(schmittTrigger(0.7));
    EXPECT_FALSE(schmittTrigger.get());

    EXPECT_FALSE(schmittTrigger(0.4));
    EXPECT_FALSE(schmittTrigger.get());

    EXPECT_FALSE(schmittTrigger(0.3));
    EXPECT_FALSE(schmittTrigger.get());
}

TEST(SchmittTriggerTest, NoTriggerOnStayingBelowLowerThreshold) {
    SchmittTrigger<double> schmittTrigger(0.5);

    EXPECT_FALSE(schmittTrigger(0.7));
    EXPECT_FALSE(schmittTrigger.get());

    EXPECT_FALSE(schmittTrigger(0.4));
    EXPECT_FALSE(schmittTrigger.get());

    EXPECT_FALSE(schmittTrigger(0.3));
    EXPECT_FALSE(schmittTrigger.get());
}

TEST(SchmittTriggerTest, NoTriggerOnTransitionWithinHysteresis) {
    SchmittTrigger<double> schmittTrigger(0.5);

    EXPECT_FALSE(schmittTrigger(0.2));
    EXPECT_FALSE(schmittTrigger.get());

    EXPECT_FALSE(schmittTrigger(0.4));
    EXPECT_FALSE(schmittTrigger.get());

    EXPECT_FALSE(schmittTrigger(0.7));
    EXPECT_FALSE(schmittTrigger.get());

    EXPECT_FALSE(schmittTrigger(0.6));
    EXPECT_FALSE(schmittTrigger.get());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}