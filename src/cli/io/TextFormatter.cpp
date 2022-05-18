// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "io/TextFormatter.hpp"

#include <cstdio>

namespace signal_estimator {

void TextFormatter::report_latency(
    double sw_hw, double hw, int sma_window, double hw_avg) {
    printf("latency:  sw+hw %7.3fms  hw %7.3fms  hw_avg%d %7.3fms\n", sw_hw, hw,
        sma_window, hw_avg);
}

void TextFormatter::report_losses(
    double loss_rate, int sma_window, double avg_loss_rate, double loss_ratio) {
    printf("losses:  rate %5.1f/sec  rate_avg%d %5.1f/sec  ratio %6.2f%%\n", loss_rate,
        sma_window, avg_loss_rate, loss_ratio);
}

} // namespace signal_estimator
