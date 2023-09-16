// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "reports/IReporter.hpp"

namespace signal_estimator {

class JsonReporter : public IReporter {
public:
    JsonReporter();

    JsonReporter(const JsonReporter&) = delete;
    JsonReporter& operator=(const JsonReporter&) = delete;

    ~JsonReporter() override;

    void report_latency(double sw_hw, double hw, int sma_window, double hw_avg) override;
    void report_losses(double loss_rate, int sma_window, double avg_loss_rate,
        double loss_ratio) override;

private:
    bool first_output_;
};

} // namespace signal_estimator
