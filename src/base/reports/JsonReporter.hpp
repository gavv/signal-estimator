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

    void report_latency(double sw_hw, double hw, double hw_avg, int sma_window) override;
    void report_losses(double loss_rate, double avg_loss_rate, double loss_ratio,
        int sma_window) override;
    void report_jitter(double swdev_avg, double swdev_per, double hwdev_avg,
        double hwdev_per, double hwbuf_avg, double hwbuf_per, int percentile) override;

private:
    bool first_output_;
};

} // namespace signal_estimator
