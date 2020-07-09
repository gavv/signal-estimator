/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "LatencyEstimator.hpp"
#include "Time.hpp"
#include "IFormatter.hpp"

namespace signal_estimator {

LatencyEstimator::StrikeTrigger::StrikeTrigger(const Config& config)
    : config_(config)
    , runmax_(config.strike_detection_window)
    , schmitt_(config.strike_detection_threshold) {
}

void LatencyEstimator::StrikeTrigger::add_frame(Frame& frame) {
    auto frame_data = frame.data();
    auto frame_size = frame.size();

    for (size_t n = 0; n < frame_size; n++) {
        auto s = double(frame_data[n]);

        s = std::abs(s);
        s = runmax_.add(s);

        if (schmitt_.add(s)) {
            last_trigger_ts_.sw_hw = (double)frame.sw_frame_time() / 1000000.0;
            last_trigger_ts_.hw = (double)frame.hw_sample_time(n) / 1000000.0;
        }
    }
}

LatencyEstimator::LatencyEstimator(const Config& config, std::unique_ptr<IFormatter>& formatter)
    : config_(config)
    , output_trigger_(config_)
    , input_trigger_(config_)
    , sma_(config.sma_window)
    , format_(formatter) {
}

LatencyEstimator::~LatencyEstimator(){
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

    if (!output_ts_.is_equal(output_trigger_.last_trigger_ts())) {
        output_ts_ = output_trigger_.last_trigger_ts();
        return check_strike_(report);
    }

    return false;
}

bool LatencyEstimator::check_input_(LatencyReport& report) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (!input_ts_.is_equal(input_trigger_.last_trigger_ts())) {
        input_ts_ = input_trigger_.last_trigger_ts();
        return check_strike_(report);
    }

    return false;
}

bool LatencyEstimator::check_strike_(LatencyReport& report) {
    if (output_ts_.is_zero() || input_ts_.is_zero()) {
        return false;
    }

    if (!(output_ts_.hw < input_ts_.hw)) {
        return false;
    }

    report.sw_hw = (input_ts_.sw_hw - output_ts_.sw_hw);
    report.hw = (input_ts_.hw - output_ts_.hw);
    report.hw_avg = sma_.add(report.hw);

    return true;
}

void LatencyEstimator::print_report_(const LatencyReport& report) {
    format_->report_latency(report.sw_hw, report.hw, (int)config_.sma_window, report.hw_avg);
}

} // namespace signal_estimator
