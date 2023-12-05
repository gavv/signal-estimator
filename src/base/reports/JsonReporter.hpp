// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Time.hpp"
#include "reports/ConsoleSink.hpp"
#include "reports/IReporter.hpp"
#include "reports/JsonPrinter.hpp"

#include <string>

namespace signal_estimator {

template <typename Sink> class BasicJsonReporter final : public IReporter {
public:
    BasicJsonReporter(
        const Config&, const DevInfo& dev_info, BasicJsonPrinter<Sink>& printer)
        : dev_name_(dev_info.short_name)
        , printer_(printer) {
    }

    void report(const LatencyReport& rep) override {
        printer_.write("{\"type\": \"latency\", \"device\": \"{}\", \"timestamp\": {},"
                       " \"sw_hw\": {}, \"hw\": {}, \"hw_avg\": {}}",
            dev_name_, (unsigned long long)wallclock_timestamp_ns(), rep.sw_hw, rep.hw,
            rep.hw_avg);
    }

    void report(const LossReport& rep) override {
        printer_.write("{\"type\": \"losses\", \"device\": \"{}\", \"timestamp\": {},"
                       " \"rate\": {}, \"rate_avg\": {}, \"ratio\": {}}",
            dev_name_, (unsigned long long)wallclock_timestamp_ns(), rep.loss_rate,
            rep.loss_rate_avg, rep.loss_ratio);
    }

    void report(const IOJitterReport& rep) override {
        printer_.write("{\"type\": \"io_jitter\", \"device\": \"{}\", \"timestamp\": {},"
                       " \"swdev_avg\": {}, \"swdev_per\": {},"
                       " \"hwdev_avg\": {}, \"hwdev_per\": {}}",
            dev_name_, (unsigned long long)wallclock_timestamp_ns(), rep.swdev_avg,
            rep.swdev_per, rep.hwdev_avg, rep.hwdev_per);
    }

    void report(const IODelayReport& rep) override {
        printer_.write("{\"type\": \"io_delay\", \"device\": \"{}\", \"timestamp\": {},"
                       " \"sw_avg\": {}, \"hw_avg\": {}}",
            dev_name_, (unsigned long long)wallclock_timestamp_ns(), rep.sw_avg,
            rep.hw_avg);
    }

private:
    const std::string dev_name_;
    BasicJsonPrinter<Sink>& printer_;
};

using JsonReporter = BasicJsonReporter<ConsoleSink>;

} // namespace signal_estimator
