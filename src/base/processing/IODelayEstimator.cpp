// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/IODelayEstimator.hpp"
#include "core/Time.hpp"
#include "reports/IReporter.hpp"

namespace signal_estimator {

IODelayEstimator::IODelayEstimator(
    const Config& config, const DevInfo& dev_info, IReporter& reporter)
    : config_(config)
    , dev_info_(dev_info)
    , thread_(&IODelayEstimator::run_, this)
    , sw_delay_avg_(config.io_delay_window)
    , hw_delay_avg_(config.io_delay_window)
    , reporter_(reporter) {
}

IODelayEstimator::~IODelayEstimator() {
    queue_.push(nullptr);

    if (thread_.joinable()) {
        thread_.join();
    }
}

void IODelayEstimator::add_output(FramePtr frame) {
    if (dev_info_.dir == Dir::Output) {
        queue_.push(std::move(frame));
    }
}

void IODelayEstimator::add_input(FramePtr frame) {
    if (dev_info_.dir == Dir::Input) {
        queue_.push(std::move(frame));
    }
}

void IODelayEstimator::run_() {
    while (auto frame = queue_.wait_pop()) {
        sw_delay_avg_.add((double)frame->sw_delay() / Millisecond);
        hw_delay_avg_.add((double)frame->hw_delay() / Millisecond);

        if (sw_delay_avg_.is_full() && report_limiter_.allow() > 0) {
            IODelayReport rep;

            rep.sw_avg = sw_delay_avg_.get();
            rep.hw_avg = hw_delay_avg_.get();

            reporter_.report(rep);
        }
    }
}

} // namespace signal_estimator
