// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "processing/IEstimator.hpp"
#include "processing/MovAvg.hpp"
#include "processing/MovMax.hpp"
#include "processing/SchmittTrigger.hpp"
#include "reports/IReporter.hpp"

#include <memory>
#include <mutex>

namespace signal_estimator {

// estimate signal latency
// assumes that the output signal was produced by StepsGenerator
// detects strikes in output and input signals and calculates the latency between them
class StepsLatencyEstimator : public IEstimator {
public:
    StepsLatencyEstimator(const Config& config, IReporter& reporter);
    ~StepsLatencyEstimator() override = default;

    StepsLatencyEstimator(const StepsLatencyEstimator&) = delete;
    StepsLatencyEstimator& operator=(const StepsLatencyEstimator&) = delete;

    void add_output(std::shared_ptr<Frame> frame) override;
    void add_input(std::shared_ptr<Frame> frame) override;

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

    class StepTrigger {
    public:
        StepTrigger(const Config& config);

        Timestamp last_trigger_ts() const {
            return last_trigger_ts_;
        }

        void add_frame(const Frame& frame);

    private:
        const Config& config_;
        MovMax<double> runmax_;
        SchmittTrigger<double> schmitt_;
        Timestamp last_trigger_ts_ {};
    };

    bool check_output_(LatencyReport&);
    bool check_input_(LatencyReport&);
    bool check_step_(LatencyReport&);

    void print_report_(const LatencyReport&);

    const Config config_;

    // accessed only by output thread
    StepTrigger output_trigger_;

    // accessed only by input thread
    StepTrigger input_trigger_;

    // accessed by both threads and protected by mutex
    std::mutex mutex_;
    Timestamp output_ts_ {};
    Timestamp input_ts_ {};
    MovAvg<double> sma_;

    IReporter& reporter_;
};

} // namespace signal_estimator
