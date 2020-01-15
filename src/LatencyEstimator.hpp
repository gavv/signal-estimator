/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"
#include "IEstimator.hpp"
#include "RunMaxCounter.hpp"
#include "SchmittTrigger.hpp"
#include "SmaCounter.hpp"

#include <mutex>

namespace signal_estimator {

class LatencyEstimator : public IEstimator {
public:
    LatencyEstimator(const Config& config);

    LatencyEstimator(const LatencyEstimator&) = delete;
    LatencyEstimator& operator=(const LatencyEstimator&) = delete;

    void add_output(Frame& frame) override;
    void add_input(Frame& frame) override;

private:
    class StrikeTrigger {
    public:
        StrikeTrigger(const Config& config);

        double last_trigger_ts() const { return last_trigger_ts_; }

        void add_frame(Frame& frame);

    private:
        const Config& config_;
        RunMaxCounter runmax_;
        SchmittTrigger schmitt_;
        double last_trigger_ts_ {};
    };

    struct LatencyReport {
        double latency_msec {};
        double avg_latency_msec {};
    };

    bool check_output_(LatencyReport&);
    bool check_input_(LatencyReport&);
    bool check_strike_(LatencyReport&);

    void print_report_(const LatencyReport&);

    const Config config_;

    // accessed only by output thread
    StrikeTrigger output_trigger_;

    // accessed only by input thread
    StrikeTrigger input_trigger_;

    // accessed by both threads and protected by mutex
    std::mutex mutex_;
    double output_ts_ {};
    double input_ts_ {};
    SmaCounter sma_;
};

} // namespace signal_estimator
