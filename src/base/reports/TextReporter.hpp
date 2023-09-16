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

    void report_latency(double sw_hw, double hw, int sma_window, double hw_avg) override;
    void report_losses(double loss_rate, int sma_window, double avg_loss_rate,
        double loss_ratio) override;
};

} // namespace signal_estimator
