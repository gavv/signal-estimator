// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/CorrelationLatencyEstimator.hpp"
#include "core/Realtime.hpp"

#include <cassert>

namespace signal_estimator {

CorrelationLatencyEstimator::CorrelationLatencyEstimator(
    const Config& config, IReporter& reporter)
    : config_(config)
    , reporter_(reporter)
    , hw_avg_(config_.report_sma_window)
    , thread_(&CorrelationLatencyEstimator::run_, this)
    , causality_timeout_lim_(config_.frames_to_ns(impulse.size()))
    , in_processor_(config)
    , out_processor_(config) {
}

CorrelationLatencyEstimator::~CorrelationLatencyEstimator() {
    if (thread_.joinable()) {
        thread_.join();
    }
}

void CorrelationLatencyEstimator::add_output(FramePtr frame) {
    queue_out_.push(frame);
}

void CorrelationLatencyEstimator::add_input(FramePtr frame) {
    queue_in_.push(frame);
}

void CorrelationLatencyEstimator::run_() {
    set_realtime();

    Timestamp in_peak, out_peak;
    double in_peak_ts = 0, out_peak_ts = 0;

    while (true) {
        while (auto out_frame = queue_out_.try_pop()) {
            if (const auto tmp_out_peak = out_processor_(*out_frame, true, 0)) {
                causality_timeout_counter_ = tmp_out_peak.hw + causality_timeout_lim_;
                out_peak = tmp_out_peak;
                out_peak_ts = tmp_out_peak.hw;
            }
        }

        if (out_peak_ts < in_peak_ts) {
            report_(out_peak, in_peak);
            out_peak_ts = in_peak_ts = 0;
        }

        do {
            auto in_frame = queue_in_.wait_pop();
            if (!in_frame) {
                return;
            }
            if (const auto tmp_in_peak
                = in_processor_(*in_frame, false, causality_timeout_counter_)) {
                in_peak = tmp_in_peak;
                in_peak_ts = tmp_in_peak.hw;
            }
        } while (!queue_in_.empty());

        if (out_peak_ts < in_peak_ts) {
            report_(out_peak, in_peak);
            out_peak_ts = in_peak_ts = 0;
        }
    }
}

void CorrelationLatencyEstimator::report_(Timestamp out_peak, Timestamp in_peak) {
    const double impulse_duration = config_.frames_to_ns(impulse.size());

    const double hw_ts = (in_peak.hw - out_peak.hw - impulse_duration) / Millisecond;
    const double swhw_ts
        = (in_peak.sw_hw - out_peak.sw_hw - impulse_duration) / Millisecond;

    reporter_.report_latency(
        swhw_ts, hw_ts, (int)config_.report_sma_window, hw_avg_(hw_ts));
}

CorrelationLatencyEstimator::Processor::Processor(const Config& config)
    : config_(config)
    , mmax_(config_.impulse_peak_detection_width)
    , mmavg_(config_.impulse_peak_detection_width) {
    std::fill(buff_.begin(), buff_.end(), 0.f);

    hw_search_start_ = 0;
    hw_search_len_
        = (config_.impulse_period * Second) - config_.frames_to_ns(impulse.size() * 2);
}

CorrelationLatencyEstimator::Timestamp CorrelationLatencyEstimator::Processor::operator()(
    const Frame& frame, const bool plain_simple, double skip_until_ts) {
    // Frames should be smaller than buffer.
    if (frame.size() / config_.n_channels > buff_len_) {
        return {};
    }

    // If one more frame won't fit into buff_ -- compute.
    Timestamp result;
    if (frame.size() / config_.n_channels > buff_len_ - inter_buff_i_) {
        if (plain_simple) {
            result = seek_max_(buff_.data(), buff_.data(), inter_buff_i_, skip_until_ts);
        } else {
            result = seek_correlation_(
                buff_.data(), buff_.data(), inter_buff_i_, skip_until_ts);
        }
        inter_buff_i_ = 0;
    }

    // Update the timestamp of the beginning of the buffer.
    buff_begin_ts_ = compute_ts_(frame, inter_buff_i_);

    // Fill buff till new samples fit inside buff_.
    // Take only left channel.
    size_t samples_in_a_frame = 0;

    for (size_t i = 0; i < frame.size(); i += config_.n_channels, samples_in_a_frame++) {
        buff_[inter_buff_i_ + samples_in_a_frame] = ((float)frame[i] / MaxSample);
    }

    inter_buff_i_ += samples_in_a_frame;
    intra_buff_counter_ += samples_in_a_frame;

    return result;
}

CorrelationLatencyEstimator::Timestamp
CorrelationLatencyEstimator::Processor::seek_correlation_(
    const float* from, float* to, const size_t sz, double skip_until_ts) {
    // FFT convolution
    optimal_filter_.perform(from, to, sz);

    // Abs(conv_out)
    std::transform(to, to + sz, to, [](const float x) { return fabs(x); });

    Timestamp res;

    for (size_t i = 0; i < sz; ++i) {
        const double cur_ts = (buff_begin_ts_.hw + config_.frames_to_ns(i));

        if (skip_until_ts > cur_ts) {
            hw_search_start_ = skip_until_ts;
            hw_search_activated_ = false;
        } else if ((hw_search_start_ + hw_search_len_) > cur_ts) {
            hw_search_activated_ = true;

            const float movmax = mmax_(to[i]);
            const float movavg = mmavg_(movmax);
            if (movmax > movavg * config_.impulse_avg_2_peak_ration_threshold
                && movmax > max_corr_val_) {
                max_corr_val_ = movmax;
                const double idx_2_ns = config_.frames_to_ns(i);
                max_corr_ts_.sw_hw = buff_begin_ts_.sw_hw + idx_2_ns;
                max_corr_ts_.hw = buff_begin_ts_.hw + idx_2_ns;
            }
        } else if (hw_search_activated_) {
            hw_search_activated_ = false;
            max_corr_val_ = 0;
            res = max_corr_ts_;
            max_corr_ts_ = { 0, 0 };
            return res;
        }
    }

    return res;
}

CorrelationLatencyEstimator::Timestamp CorrelationLatencyEstimator::Processor::seek_max_(
    const float* from, float* to, const size_t sz, double skip_until_ts) {
    Timestamp res;

    for (size_t i = 0; i < sz; ++i) {
        if (!max_timeout_activated_) {
            if (fabs(from[i]) > 1e-5
                && skip_until_ts < (buff_begin_ts_.hw + config_.frames_to_ns(i))) {
                const double idx_2_ns = config_.frames_to_ns(i);

                max_timeout_counter_ = 0;
                max_timeout_activated_ = true;

                res.sw_hw = buff_begin_ts_.sw_hw + idx_2_ns;
                res.hw = buff_begin_ts_.hw + idx_2_ns;

                to[i] = 1.f;
            } else {
                to[i] = 0.f;
            }
        } else {
            to[i] = 0.f;

            max_timeout_counter_++;
            if (max_timeout_counter_ > buff_len_ * 1.5) {
                max_timeout_activated_ = false;
            }
        }
    }
    return res;
}

CorrelationLatencyEstimator::Timestamp
CorrelationLatencyEstimator::Processor::compute_ts_(
    const Frame& frame, size_t frame_offset) const {
    const double frame_offset_ns = config_.frames_to_ns(frame_offset);

    Timestamp ts;
    ts.sw_hw = (double)frame.sw_frame_time() - frame_offset_ns;
    ts.hw = (double)frame.hw_sample_time(0) - frame_offset_ns;

    return ts;
}

} // namespace signal_estimator
