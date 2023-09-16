// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

class IReporter {
public:
    virtual ~IReporter() = default;

    virtual void report_latency(double sw_hw, double hw, int sma_window, double hw_avg)
        = 0;

    virtual void report_losses(
        double loss_rate, int sma_window, double avg_loss_rate, double loss_ratio)
        = 0;
};

} // namespace signal_estimator
