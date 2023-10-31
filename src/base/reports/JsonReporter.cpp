// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/JsonReporter.hpp"
#include "core/Time.hpp"

#include <cstdio>

namespace signal_estimator {

JsonReporter::JsonReporter(const Config&, const DevInfo& dev_info, JsonPrinter& printer)
    : printer_(printer)
    , dev_name_(dev_info.short_name) {
}

void JsonReporter::report(const LatencyReport& rep) {
    printer_.write_line("{\"type\": \"latency\", \"device\": \"%s\", \"timestamp\": %llu,"
                        " \"sw_hw\": %f, \"hw\": %f, \"hw_avg\": %f}",
        dev_name_.c_str(), (unsigned long long)wallclock_timestamp_ns(), rep.sw_hw,
        rep.hw, rep.hw_avg);
}

void JsonReporter::report(const LossReport& rep) {
    printer_.write_line("{\"type\": \"losses\", \"device\": \"%s\", \"timestamp\": %llu,"
                        " \"rate\": %f, \"rate_avg\": %f, \"ratio\": %f}",
        dev_name_.c_str(), (unsigned long long)wallclock_timestamp_ns(), rep.loss_rate,
        rep.loss_rate_avg, rep.loss_ratio);
}

void JsonReporter::report(const IOJitterReport& rep) {
    printer_.write_line(
        "{\"type\": \"io_jitter\", \"device\": \"%s\", \"timestamp\": %llu,"
        " \"swdev_avg\": %f, \"swdev_per\": %f,"
        " \"hwdev_avg\": %f, \"hwdev_per\": %f}",
        dev_name_.c_str(), (unsigned long long)wallclock_timestamp_ns(), rep.swdev_avg,
        rep.swdev_per, rep.hwdev_avg, rep.hwdev_per);
}

} // namespace signal_estimator
