// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "reports/JsonReporter.hpp"
#include "core/Time.hpp"

namespace signal_estimator {

JsonReporter::JsonReporter(const Config&, const DevInfo& dev_info, JsonPrinter& printer)
    : dev_name_(dev_info.short_name)
    , printer_(printer) {
}

void JsonReporter::report(const LatencyReport& rep) {
    printer_.println("{{\"type\": \"latency\", \"device\": \"{}\", \"timestamp\": {},"
                     " \"sw_hw\": {}, \"hw\": {}, \"hw_avg\": {}}}",
        dev_name_, wallclock_timestamp_ns(), rep.sw_hw, rep.hw, rep.hw_avg);
}

void JsonReporter::report(const LossReport& rep) {
    printer_.println("{{\"type\": \"losses\", \"device\": \"{}\", \"timestamp\": {},"
                     " \"rate\": {}, \"rate_avg\": {}, \"ratio\": {}}}",
        dev_name_, wallclock_timestamp_ns(), rep.loss_rate, rep.loss_rate_avg,
        rep.loss_ratio);
}

void JsonReporter::report(const IOJitterReport& rep) {
    printer_.println("{{\"type\": \"io_jitter\", \"device\": \"{}\", \"timestamp\": {},"
                     " \"swdev_avg\": {}, \"swdev_per\": {},"
                     " \"hwdev_avg\": {}, \"hwdev_per\": {}}}",
        dev_name_, wallclock_timestamp_ns(), rep.swdev_avg, rep.swdev_per, rep.hwdev_avg,
        rep.hwdev_per);
}

void JsonReporter::report(const IODelayReport& rep) {
    printer_.println("{{\"type\": \"io_delay\", \"device\": \"{}\", \"timestamp\": {},"
                     " \"sw_avg\": {}, \"hw_avg\": {}}}",
        dev_name_, wallclock_timestamp_ns(), rep.sw_avg, rep.hw_avg);
}

} // namespace signal_estimator
