/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"
#include "SchmittTrigger.hpp"
#include "SmaCounter.hpp"
#include "Time.hpp"

#include <cstdint>
#include <cstdlib>
#include <mutex>

namespace signal_estimator {

class LatencyEstimator {
public:
    LatencyEstimator(const Config& config);

    LatencyEstimator(const LatencyEstimator&) = delete;
    LatencyEstimator& operator=(const LatencyEstimator&) = delete;

    void add_output(nanoseconds_t ts, const int16_t* buf, size_t bufsz);
    void add_input(nanoseconds_t ts, const int16_t* buf, size_t bufsz);

private:
    struct Report {
        double latency_msec {};
        double avg_latency_msec {};
    };

    bool process_output_(Report&);
    bool process_input_(Report&);

    bool check_triggers_(Report&);
    void print_report_(const Report&);

    const Config config_;

    // accessed only by output thread
    SchmittTrigger output_trigger_;

    // accessed only by input thread
    SchmittTrigger input_trigger_;

    // all fields below are accessed by both threads and are guarded by mutex
    bool output_triggered_ {};
    bool input_triggered_ {};

    double output_ts_ {};
    double input_ts_ {};

    bool latency_reported_ {};

    SmaCounter sma_;

    std::mutex mutex_;
};

} // namespace signal_estimator
