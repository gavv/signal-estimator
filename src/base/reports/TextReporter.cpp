// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/TextReporter.hpp"

#include <cstdio>

namespace signal_estimator {

TextReporter::TextReporter(const Config& config, const DevInfo& dev_info)
    : config_(config)
    , sign_(config.diff_inputs) {
    if (config.show_device_names && !dev_info.short_name.empty()) {
        suffix_ = "[" + dev_info.short_name + "]";
    }
}

void TextReporter::report(const LatencyReport& rep) {
    if (sign_) {
        printf("latency%s:"
               "  sw+hw %+7.3fms  hw %+7.3fms  hw_avg%d %+7.3fms\n",
            suffix_.c_str(), rep.sw_hw, rep.hw, (int)config_.report_sma_window,
            rep.hw_avg);
    } else {
        printf("latency%s:"
               "  sw+hw %7.3fms  hw %7.3fms  hw_avg%d %7.3fms\n",
            suffix_.c_str(), rep.sw_hw, rep.hw, (int)config_.report_sma_window,
            rep.hw_avg);
    }
}

void TextReporter::report(const LossReport& rep) {
    if (sign_) {
        printf("losses%s:"
               "  rate %+5.1f/sec  rate_avg%d %+5.1f/sec  ratio %+6.2f%%\n",
            suffix_.c_str(), rep.loss_rate, (int)config_.report_sma_window,
            rep.loss_rate_avg, rep.loss_ratio);
    } else {
        printf("losses%s:"
               "  rate %5.1f/sec  rate_avg%d %5.1f/sec  ratio %6.2f%%\n",
            suffix_.c_str(), rep.loss_rate, (int)config_.report_sma_window,
            rep.loss_rate_avg, rep.loss_ratio);
    }
}

void TextReporter::report(const IOJitterReport& rep) {
    printf("io_jitter%s:"
           "  sw_avg %6.3fms  sw_p%d %6.3fms"
           "  hw_avg %6.3fms  hw_p%d %6.3fms\n",
        suffix_.c_str(), rep.swdev_avg, (int)config_.io_jitter_percentile, rep.swdev_per,
        rep.hwdev_avg, (int)config_.io_jitter_percentile, rep.hwdev_per);
}

} // namespace signal_estimator
