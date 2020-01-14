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
            trigger_ts_msec_ = (double)frame.sample_time(n) / 1000000.0;
        }
    }
}

bool LatencyEstimator::StrikeTrigger::is_triggered() const {
    return schmitt_.is_triggered();
}

double LatencyEstimator::StrikeTrigger::trigger_ts_msec() const {
    return trigger_ts_msec_;
}

LatencyEstimator::LatencyEstimator(const Config& config)
    : config_(config)
    , output_trigger_(config_)
    , input_trigger_(config_)
    , sma_(config.sma_window) {
}

void LatencyEstimator::add_output(Frame& frame) {
    output_trigger_.add_frame(frame);

    Report report;

    if (process_output_(report)) {
        print_report_(report);
    }
}

void LatencyEstimator::add_input(Frame& frame) {
    input_trigger_.add_frame(frame);

    Report report;

    if (process_input_(report)) {
        print_report_(report);
    }
}

bool LatencyEstimator::process_output_(Report& report) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (output_trigger_.is_triggered()) {
        if (!output_triggered_) {
            output_triggered_ = true;
            output_ts_ = output_trigger_.trigger_ts_msec();
            return check_triggers_(report);
        }
    } else {
        output_triggered_ = false;
        latency_reported_ = false;
    }

    return false;
}

bool LatencyEstimator::process_input_(Report& report) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (input_trigger_.is_triggered()) {
        if (!input_triggered_) {
            input_triggered_ = true;
            input_ts_ = input_trigger_.trigger_ts_msec();
            return check_triggers_(report);
        }
    } else {
        input_triggered_ = false;
    }

    return false;
}

bool LatencyEstimator::check_triggers_(Report& report) {
    if (!output_triggered_ || !input_triggered_) {
        return false;
    }

    if (!(output_ts_ < input_ts_)) {
        return false;
    }

    if (latency_reported_) {
        return false;
    }

    report.latency_msec = (input_ts_ - output_ts_);
    report.avg_latency_msec = sma_.add(report.latency_msec);

    latency_reported_ = true;
    return true;
}

void LatencyEstimator::print_report_(const Report& report) {
    se_log_info("latency:  cur %7.3fms  avg%d %7.3fms", report.latency_msec,
        (int)config_.sma_window, report.avg_latency_msec);
}

} // namespace signal_estimator
