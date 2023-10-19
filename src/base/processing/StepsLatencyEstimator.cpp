// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/StepsLatencyEstimator.hpp"
#include "core/Time.hpp"
#include "reports/IReporter.hpp"

namespace signal_estimator {

StepsLatencyEstimator::StepTrigger::StepTrigger(const Config& config)
    : config_(config)
    , runmax_(config.step_detection_window)
    , schmitt_(config.step_detection_threshold) {
}

void StepsLatencyEstimator::StepTrigger::add_frame(const Frame& frame) {
    for (size_t n = 0; n < frame.size(); n++) {
        auto s = double(frame[n]);

        s = std::abs(s);
        s = runmax_(s);

        if (schmitt_(s)) {
            last_trigger_ts_.sw_hw = (double)frame.sw_frame_time() / 1000000.0;
            last_trigger_ts_.hw = (double)frame.hw_sample_time(n) / 1000000.0;
        }
    }
}

StepsLatencyEstimator::StepsLatencyEstimator(const Config& config, IReporter& reporter)
    : config_(config)
    , output_trigger_(config_)
    , input_trigger_(config_)
    , sma_(config.report_sma_window)
    , reporter_(reporter) {
}

void StepsLatencyEstimator::add_output(FramePtr frame) {
    if (!frame) {
        return;
    }
    output_trigger_.add_frame(*frame);

    LatencyReport report;

    if (check_output_(report)) {
        print_report_(report);
    }
    frame.reset();
}

void StepsLatencyEstimator::add_input(FramePtr frame) {
    if (!frame) {
        return;
    }
    input_trigger_.add_frame(*frame);

    LatencyReport report;

    if (check_input_(report)) {
        print_report_(report);
    }
}

bool StepsLatencyEstimator::check_output_(LatencyReport& report) {
    std::unique_lock lock(mutex_);

    if (!output_ts_.is_equal(output_trigger_.last_trigger_ts())) {
        output_ts_ = output_trigger_.last_trigger_ts();
        return check_step_(report);
    }

    return false;
}

bool StepsLatencyEstimator::check_input_(LatencyReport& report) {
    std::unique_lock lock(mutex_);

    if (!input_ts_.is_equal(input_trigger_.last_trigger_ts())) {
        input_ts_ = input_trigger_.last_trigger_ts();
        return check_step_(report);
    }

    return false;
}

bool StepsLatencyEstimator::check_step_(LatencyReport& report) {
    if (output_ts_.is_zero() || input_ts_.is_zero()) {
        return false;
    }

    if (!(output_ts_.hw < input_ts_.hw)) {
        return false;
    }

    report.sw_hw = (input_ts_.sw_hw - output_ts_.sw_hw);
    report.hw = (input_ts_.hw - output_ts_.hw);
    report.hw_avg = sma_(report.hw);

    return true;
}

void StepsLatencyEstimator::print_report_(const LatencyReport& report) {
    reporter_.report_latency(
        report.sw_hw, report.hw, (int)config_.report_sma_window, report.hw_avg);
}

} // namespace signal_estimator
