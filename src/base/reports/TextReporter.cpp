// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/TextReporter.hpp"

#include <cstdio>

namespace signal_estimator {

void TextReporter::report_latency(
    double sw_hw, double hw, double hw_avg, int sma_window) {
    printf("latency:"
           "  sw+hw %7.3fms  hw %7.3fms  hw_avg%d %7.3fms\n",
        sw_hw, hw, sma_window, hw_avg);
}

void TextReporter::report_losses(
    double loss_rate, double avg_loss_rate, double loss_ratio, int sma_window) {
    printf("losses:"
           "  rate %5.1f/sec  rate_avg%d %5.1f/sec  ratio %6.2f%%\n",
        loss_rate, sma_window, avg_loss_rate, loss_ratio);
}

void TextReporter::report_jitter(double swdev_avg, double swdev_per, double hwdev_avg,
    double hwdev_per, double hwbuf_avg, double hwbuf_per, int percentile) {
    printf("jitter:"
           "  sw avg %6.3fms p%d %6.3fms"
           "  hw avg %6.3fms p%d %6.3fms"
           "  buf avg %6.3fms p%d %6.3fms\n",
        swdev_avg, percentile, swdev_per, hwdev_avg, percentile, hwdev_per, hwbuf_avg,
        percentile, hwbuf_per);
}

} // namespace signal_estimator
