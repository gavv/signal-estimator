// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

class IReporter {
public:
    virtual ~IReporter() = default;

    virtual void report_latency(double sw_hw, double hw, double hw_avg, int sma_window)
        = 0;

    virtual void report_losses(
        double loss_rate, double avg_loss_rate, double loss_ratio, int sma_window)
        = 0;

    virtual void report_jitter(double swdev_avg, double swdev_per, double hwdev_avg,
        double hwdev_per, double hwbuf_avg, double hwbuf_per, int percentile)
        = 0;
};

} // namespace signal_estimator
