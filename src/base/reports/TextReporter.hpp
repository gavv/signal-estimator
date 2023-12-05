// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/DevInfo.hpp"
#include "reports/IReporter.hpp"
#include "reports/TextPrinter.hpp"

#include <cstdio>
#include <string>

namespace signal_estimator {

template <typename Sink> class BasicTextReporter final : public IReporter {
public:
    BasicTextReporter(
        const Config& config, const DevInfo& dev_info, BasicTextPrinter<Sink>& printer)
        : config_(config)
        , sign_(config.diff_inputs)
        , printer_(printer) {
        if (config.show_device_names && !dev_info.short_name.empty()) {
            suffix_ = "[" + dev_info.short_name + "]";
        }
    }

    void report(const LatencyReport& rep) override {
        if (sign_) {
            printer_.write("latency{}:"
                           "  sw+hw {:+7.3}fms  hw {:+7.3}fms  hw_avg{} {:+7.3}fms\n",
                suffix_, rep.sw_hw, rep.hw, (int)config_.report_sma_window, rep.hw_avg);
        } else {
            printer_.write("latency{}:"
                           "  sw+hw {:7.3}fms  hw {:7.3}fms  hw_avg{} {:7.3}fms\n",
                suffix_, rep.sw_hw, rep.hw, (int)config_.report_sma_window, rep.hw_avg);
        }
    }

    void report(const LossReport& rep) override {
        if (sign_) {
            printer_.write(
                "losses{}:"
                "  rate {:+5.1}f/sec  rate_avg{} {:+5.1}f/sec  ratio {:+6.2f}%\n",
                suffix_, rep.loss_rate, (int)config_.report_sma_window, rep.loss_rate_avg,
                rep.loss_ratio);
        } else {
            printer_.write("losses{}:"
                           "  rate {:5.1}f/sec  rate_avg{} {:5.1}f/sec  ratio {:6.2f}%\n",
                suffix_, rep.loss_rate, (int)config_.report_sma_window, rep.loss_rate_avg,
                rep.loss_ratio);
        }
    }

    void report(const IOJitterReport& rep) override {
        printer_.write(
            "io_jitter{}:"
            "  sw_avg {:7.3}fms  sw_p{} {:7.3}fms  hw_avg {:7.3}fms  hw_p{} {:7.3}fms\n",
            suffix_, rep.swdev_avg, (int)config_.io_jitter_percentile, rep.swdev_per,
            rep.hwdev_avg, (int)config_.io_jitter_percentile, rep.hwdev_per);
    }

    void report(const IODelayReport& rep) override {
        printer_.write("io_delay{}:"
                       "  sw_avg {:7.3}fms  hw_avg {:7.3}fms\n",
            suffix_, rep.sw_avg, rep.hw_avg);
    }

private:
    Config config_;
    bool sign_ {};
    std::string suffix_;
    BasicTextPrinter<Sink>& printer_;
};

using TextReporter = BasicTextReporter<ConsoleSink>;

} // namespace signal_estimator
