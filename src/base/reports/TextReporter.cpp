// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/TextReporter.hpp"

#include <cstdio>

namespace signal_estimator {

TextReporter::TextReporter(
    const Config& config, const DevInfo& dev_info, TextPrinter& printer)
    : config_(config)
    , sign_(config.diff_inputs)
    , printer_(printer) {
    if (config.show_device_names && !dev_info.short_name.empty()) {
        suffix_ = "[" + dev_info.short_name + "]";
    }
}

void TextReporter::report(const LatencyReport& rep) {
    if (sign_) {
        printer_.println("latency{}:"
                         "  sw+hw {:+7.3}ms  hw {:+7.3}ms  hw_avg{} {:+7.3}ms",
            suffix_, rep.sw_hw, rep.hw, config_.report_sma_window, rep.hw_avg);
    } else {
        printer_.println("latency{}:"
                         "  sw+hw {:7.3}ms  hw {:7.3}ms  hw_avg{} {:7.3}ms",
            suffix_, rep.sw_hw, rep.hw, config_.report_sma_window, rep.hw_avg);
    }
}

void TextReporter::report(const LossReport& rep) {
    if (sign_) {
        printer_.println("losses{}:"
                         "  rate {:+5.1}/sec  rate_avg{} {:+5.1}/sec  ratio {:+6.2}%",
            suffix_, rep.loss_rate, config_.report_sma_window, rep.loss_rate_avg,
            rep.loss_ratio);
    } else {
        printer_.println("losses{}:"
                         "  rate {:5.1}/sec  rate_avg{} {:5.1}/sec  ratio {:6.2}%",
            suffix_, rep.loss_rate, config_.report_sma_window, rep.loss_rate_avg,
            rep.loss_ratio);
    }
}

void TextReporter::report(const IOJitterReport& rep) {
    printer_.println(
        "io_jitter{}:"
        "  sw_avg {:7.3}ms  sw_p{} {:7.3}ms  hw_avg {:7.3}ms  hw_p{} {:7.3}ms",
        suffix_, rep.swdev_avg, config_.io_jitter_percentile, rep.swdev_per,
        rep.hwdev_avg, config_.io_jitter_percentile, rep.hwdev_per);
}

void TextReporter::report(const IODelayReport& rep) {
    printer_.println("io_delay{}:"
                     "  sw_avg {:7.3}ms  hw_avg {:7.3}ms",
        suffix_, rep.sw_avg, rep.hw_avg);
}

} // namespace signal_estimator
