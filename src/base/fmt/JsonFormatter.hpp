// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "fmt/IFormatter.hpp"

namespace signal_estimator {

class JsonFormatter : public IFormatter {
public:
    JsonFormatter();

    JsonFormatter(const JsonFormatter&) = delete;
    JsonFormatter& operator=(const JsonFormatter&) = delete;

    ~JsonFormatter() override;

    void report_latency(double sw_hw, double hw, int sma_window, double hw_avg) override;
    void report_losses(double loss_rate, int sma_window, double avg_loss_rate,
        double loss_ratio) override;

private:
    bool first_output_;
};

} // namespace signal_estimator
