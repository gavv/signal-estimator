// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "reports/IReporter.hpp"

namespace signal_estimator {

class TextReporter : public IReporter {
public:
    TextReporter() = default;
    ~TextReporter() override = default;

    TextReporter(const TextReporter&) = delete;
    TextReporter& operator=(const TextReporter&) = delete;

    void report_latency(double sw_hw, double hw, double hw_avg, int sma_window) override;
    void report_losses(double loss_rate, double avg_loss_rate, double loss_ratio,
        int sma_window) override;
    void report_jitter(double swdev_avg, double swdev_per, double hwdev_avg,
        double hwdev_per, double hwbuf_avg, double hwbuf_per, int percentile) override;
};

} // namespace signal_estimator
