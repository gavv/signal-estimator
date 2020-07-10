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
#include "IFormatter.hpp"

#include <memory>
#include <mutex>

namespace signal_estimator {

// estimate signal latency
// assumes that the output signal was produced by StrikeGenerator
// detects strikes in output and input signals and calculates the latency between them
class LatencyEstimator : public IEstimator {
public:
    LatencyEstimator(const Config& config, const std::unique_ptr<IFormatter>& formatter);

    LatencyEstimator(const LatencyEstimator&) = delete;
    LatencyEstimator& operator=(const LatencyEstimator&) = delete;

    ~LatencyEstimator();

    void add_output(Frame& frame) override;
    void add_input(Frame& frame) override;

private:
    struct LatencyReport {
        double sw_hw {};
        double hw {};
        double hw_avg {};
    };

    struct Timestamp {
        double sw_hw {};
        double hw {};

        bool is_zero() const {
            return sw_hw == 0 && hw == 0;
        }

        bool is_equal(const Timestamp& other) const {
            return sw_hw == other.sw_hw && hw == other.hw;
        }
    };

    class StrikeTrigger {
    public:
        StrikeTrigger(const Config& config);

        Timestamp last_trigger_ts() const { return last_trigger_ts_; }

        void add_frame(Frame& frame);

    private:
        const Config& config_;
        RunMaxCounter runmax_;
        SchmittTrigger schmitt_;
        Timestamp last_trigger_ts_ {};
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
    Timestamp output_ts_ {};
    Timestamp input_ts_ {};
    SmaCounter sma_;

    const std::unique_ptr<IFormatter>& format_;
};

} // namespace signal_estimator
