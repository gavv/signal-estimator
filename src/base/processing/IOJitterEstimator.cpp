// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/IOJitterEstimator.hpp"
#include "core/Time.hpp"

namespace signal_estimator {

IOJitterEstimator::JitterStats::JitterStats(const Config& config, const DevInfo& dev_info)
    : dev_avg(config.io_jitter_window)
    , dev_per(config.io_jitter_window, config.io_jitter_percentile / 100.)
    , ideal_period((double)config.samples_to_ns(dev_info.period_size) / Millisecond) {
}

void IOJitterEstimator::JitterStats::update(nanoseconds_t next_ts) {
    if (prev_ts) {
        // delta between two frames
        const double delta = double(next_ts - prev_ts) / Millisecond;
        // deviation of delta from ideal period
        const double deviation = std::abs(delta - ideal_period);
        // statistics
        dev_avg.add(deviation);
        dev_per.add(deviation);
    }
    prev_ts = next_ts;
}

IOJitterEstimator::BufStats::BufStats(const Config& config)
    : len_avg(config.io_jitter_window)
    , len_per(config.io_jitter_window, config.io_jitter_percentile / 100.) {
}

void IOJitterEstimator::BufStats::update(nanoseconds_t buf_len) {
    const double len_ms = (double)buf_len / Millisecond;

    len_avg.add(len_ms);
    len_per.add(len_ms);
}

IOJitterEstimator::IOJitterEstimator(
    const Config& config, const DevInfo& dev_info, Dir dir, IReporter& reporter)
    : config_(config)
    , dev_info_(dev_info)
    , dir_(dir)
    , thread_(&IOJitterEstimator::run_, this)
    , sw_stats_(config, dev_info)
    , hw_stats_(config, dev_info)
    , buf_stats_(config)
    , reporter_(reporter) {
}

IOJitterEstimator::~IOJitterEstimator() {
    if (thread_.joinable()) {
        thread_.join();
    }
}

void IOJitterEstimator::add_output(FramePtr frame) {
    if (dir_ == Dir::Output) {
        queue_.push(frame);
    }
}

void IOJitterEstimator::add_input(FramePtr frame) {
    if (dir_ == Dir::Input) {
        queue_.push(frame);
    }
}

void IOJitterEstimator::run_() {
    while (auto frame = queue_.wait_pop()) {
        sw_stats_.update(frame->sw_frame_time());
        hw_stats_.update(frame->hw_frame_time());
        buf_stats_.update(frame->hw_buf_len());

        if (buf_stats_.len_avg.is_full() && report_limiter_.allow() > 0) {
            reporter_.report_jitter(sw_stats_.dev_avg.get(), sw_stats_.dev_per.get(),
                hw_stats_.dev_avg.get(), hw_stats_.dev_per.get(),
                buf_stats_.len_avg.get(), buf_stats_.len_per.get(),
                (int)config_.io_jitter_percentile);
        }
    }
}

} // namespace signal_estimator
