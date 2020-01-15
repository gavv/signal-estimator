/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "LatencyEstimator.hpp"
#include "Log.hpp"
#include "Time.hpp"

namespace signal_estimator {

LatencyEstimator::StrikeTrigger::StrikeTrigger(const Config& config)
    : config_(config)
    , runmax_(config.strike_window)
    , schmitt_(config.strike_threshold) {
}

void LatencyEstimator::StrikeTrigger::add_frame(Frame& frame) {
    auto frame_data = frame.data();
    auto frame_size = frame.size();

    for (size_t n = 0; n < frame_size; n++) {
        auto s = double(frame_data[n]);

        s = std::abs(s);
        s = runmax_.add(s);

        if (schmitt_.add(s)) {
            last_trigger_ts_ = (double)frame.sample_time(n) / 1000000.0;
        }
    }
}

LatencyEstimator::LatencyEstimator(const Config& config)
    : config_(config)
    , output_trigger_(config_)
    , input_trigger_(config_)
    , sma_(config.sma_window) {
}

void LatencyEstimator::add_output(Frame& frame) {
    output_trigger_.add_frame(frame);

    LatencyReport report;

    if (check_output_(report)) {
        print_report_(report);
    }
}

void LatencyEstimator::add_input(Frame& frame) {
    input_trigger_.add_frame(frame);

    LatencyReport report;

    if (check_input_(report)) {
        print_report_(report);
    }
}

bool LatencyEstimator::check_output_(LatencyReport& report) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (output_ts_ != output_trigger_.last_trigger_ts()) {
        output_ts_ = output_trigger_.last_trigger_ts();
        return check_strike_(report);
    }

    return false;
}

bool LatencyEstimator::check_input_(LatencyReport& report) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (input_ts_ != input_trigger_.last_trigger_ts()) {
        input_ts_ = input_trigger_.last_trigger_ts();
        return check_strike_(report);
    }

    return false;
}

bool LatencyEstimator::check_strike_(LatencyReport& report) {
    if (output_ts_ == 0 || input_ts_ == 0) {
        return false;
    }

    if (!(output_ts_ < input_ts_)) {
        return false;
    }

    report.latency_msec = (input_ts_ - output_ts_);
    report.avg_latency_msec = sma_.add(report.latency_msec);

    return true;
}

void LatencyEstimator::print_report_(const LatencyReport& report) {
    se_log_info("latency:  cur %7.3fms  avg%d %7.3fms", report.latency_msec,
        (int)config_.sma_window, report.avg_latency_msec);
}

} // namespace signal_estimator
