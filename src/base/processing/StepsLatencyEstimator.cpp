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
        auto s = (float)frame[n];

        s = std::abs(s);
        s = runmax_(s);

        if (schmitt_(s)) {
            last_trigger_ts_.sw_hw = (double)frame.sw_sample_time(n) / Millisecond;
            last_trigger_ts_.hw = (double)frame.hw_sample_time(n) / Millisecond;
        }
    }
}

StepsLatencyEstimator::StepsLatencyEstimator(const Config& config, IReporter& reporter)
    : config_(config)
    , thread_(&StepsLatencyEstimator::run_, this)
    , output_trigger_(config_)
    , input_trigger_(config_)
    , sma_(config.report_sma_window)
    , reporter_(reporter) {
}

StepsLatencyEstimator::~StepsLatencyEstimator() {
    queue_in_.push(nullptr);

    if (thread_.joinable()) {
        thread_.join();
    }
}

void StepsLatencyEstimator::add_output(FramePtr frame) {
    queue_out_.push(std::move(frame));
}

void StepsLatencyEstimator::add_input(FramePtr frame) {
    queue_in_.push(std::move(frame));
}

void StepsLatencyEstimator::run_() {
    while (true) {
        while (auto out_frame = queue_out_.try_pop()) {
            output_trigger_.add_frame(*out_frame);

            LatencyReport rep;
            if (check_output_(rep)) {
                reporter_.report(rep);
            }
        }

        do {
            auto in_frame = queue_in_.wait_pop();
            if (!in_frame) {
                return;
            }
            input_trigger_.add_frame(*in_frame);

            LatencyReport rep;
            if (check_input_(rep)) {
                reporter_.report(rep);
            }
        } while (!queue_in_.empty());
    }
}

bool StepsLatencyEstimator::check_output_(LatencyReport& report) {
    if (!output_ts_.is_equal(output_trigger_.last_trigger_ts())) {
        output_ts_ = output_trigger_.last_trigger_ts();
        return check_step_(report);
    }
    return false;
}

bool StepsLatencyEstimator::check_input_(LatencyReport& report) {
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

    if (std::abs(input_ts_.hw - output_ts_.hw) > (double)config_.step_interval * 1000) {
        return false;
    }

    report.sw_hw = (input_ts_.sw_hw - output_ts_.sw_hw);
    report.hw = (input_ts_.hw - output_ts_.hw);
    report.hw_avg = sma_(report.hw);

    return true;
}

} // namespace signal_estimator
