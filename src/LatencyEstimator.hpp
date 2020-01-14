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

        void add_frame(Frame& frame);

        bool is_triggered() const;
        double trigger_ts_msec() const;

    private:
        const Config& config_;
        RunMaxCounter runmax_;
        SchmittTrigger schmitt_;
        double trigger_ts_msec_ {};
    };

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
    StrikeTrigger output_trigger_;

    // accessed only by input thread
    StrikeTrigger input_trigger_;

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
