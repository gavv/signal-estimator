// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "processing/ConvolutionLatencyEstimator.hpp"
#include "core/Pool.hpp"
#include "core/Realtime.hpp"

namespace signal_estimator {

static inline double i2ns(const size_t i, const Config &config) {
    return double(i)/double(config.sample_rate) * 1e9;
}

ConvolutionLatencyEstimator::ConvolutionLatencyEstimator(
    const Config& config, IFormatter& formatter)
    : config_(config)
    , formatter_(formatter)
    , hw_avg_(config_.report_sma_window)
    , thread_(&ConvolutionLatencyEstimator::run, this)
    , causality_timeout_lim_(i2ns(impulse.size() - config_.impulse_peak_detection_width - 1,config_))
    , in_processor_(config)
    , out_processor_(config) {
}

ConvolutionLatencyEstimator::~ConvolutionLatencyEstimator() {
    if (thread_.joinable()) {
        thread_.join();
    }
}

void ConvolutionLatencyEstimator::add_output(std::shared_ptr<Frame> frame) {
    queue_out_.push(frame);
}

void ConvolutionLatencyEstimator::add_input(std::shared_ptr<Frame> frame) {
    queue_in_.push(frame);
}

void ConvolutionLatencyEstimator::run() {
    Timestamp outpeak, inpeak;
    double last_in_peak = 0, last_out_peak = 0;
    set_realtime();

    while (true) {

        while (auto out_f = queue_out_.pop(false)) {
            std::shared_ptr<Frame> out_frame = *out_f;
            if (!out_frame) {
                return;
            }
            if (const auto tmp_out_rep = out_processor_(*out_frame, true, 0)) {
                causality_timeoute_counter_
                    = tmp_out_rep.hw + causality_timeout_lim_;
                outpeak = tmp_out_rep;
                last_out_peak = outpeak.hw;
            }
            out_frame.reset();
        }

        if (last_out_peak < last_in_peak) {
            const double imp_duration
                = i2ns(impulse.size(), config_);
            const double hw_ts = (inpeak.hw - outpeak.hw - imp_duration)*1e-6;
            formatter_.report_latency(
                (inpeak.sw_hw - outpeak.sw_hw - imp_duration) * 1e-6, hw_ts,
                (int)config_.report_sma_window, hw_avg_(hw_ts));
            last_out_peak = last_in_peak = 0;
        }

        // Acquire new frame.
        do {
            auto in_f = queue_in_.pop(true);
            Frame* in_frame = in_f->get();
            if (!in_frame) {
                return;
            }
            if (const auto tmp_in_reg
                = in_processor_(*in_frame, false, causality_timeoute_counter_)) {
                inpeak = tmp_in_reg;
                last_in_peak = inpeak.hw;
            }
        } while (queue_in_.free_size() < queue_sz_);

        if (last_out_peak < last_in_peak) {
            const double imp_duration
                = i2ns(impulse.size(), config_);
            const double hw_ts = (inpeak.hw - outpeak.hw - imp_duration)*1e-6;
            formatter_.report_latency(
                (inpeak.sw_hw - outpeak.sw_hw - imp_duration) * 1e-6, hw_ts,
                (int)config_.report_sma_window, hw_avg_(hw_ts));
            last_out_peak = last_in_peak = 0;
        }
    }
}

ConvolutionLatencyEstimator::Processor::Processor(const Config& config)
    : config_(config)
    , mmax_(config_.impulse_peak_detection_width)
    , buff_()
    , inter_buff_i_(0)
    , intra_buff_counter_(0) {
    std::fill(buff_.begin(), buff_.end(), 0.f);
    (void)buff_;
}

ConvolutionLatencyEstimator::Timestamp ConvolutionLatencyEstimator::Processor::operator()(
    Frame& frame, const bool plain_simple, double skip_until_ts) {
    ConvolutionLatencyEstimator::Timestamp result;
    // Fill buff till new samples fit inside buff_.
    // Take only left channel.
    assert(frame.size() <= buff_len_);
    size_t samples_in_a_frame = 0;
    // Update the timestamp of the beginning of the buffer.
    buff_begin_ts_ = update_ts(frame, inter_buff_i_);

    for (size_t i = 0; i < frame.size(); i += config_.n_channels, samples_in_a_frame++) {
        buff_[inter_buff_i_ + samples_in_a_frame]
            = ((float)*(frame.data() + i) / 32768.f);
    }

    inter_buff_i_ += samples_in_a_frame;
    intra_buff_counter_ += samples_in_a_frame;

    // If one more frame won't fit into buff_ -- compute.
    // (Assume that frame length is constant across the session).
    if (buff_len_ - inter_buff_i_ < samples_in_a_frame) {
        if (plain_simple) {
            result = seek_max(buff_.data(), buff_.data(), inter_buff_i_, skip_until_ts);
        } else {
            result
                = process_buff(buff_.data(), buff_.data(), inter_buff_i_, skip_until_ts);
        }
        inter_buff_i_ = 0;
    }
    return result;
}

ConvolutionLatencyEstimator::Timestamp ConvolutionLatencyEstimator::Processor::process_buff(
    const float* from, float* to, const size_t sz, double skip_until_ts) {

    static double hw_search_start_ = 0;
    static double hw_search_len_ = config_.impulse_period * 1e9 - double (impulse.size()*2) / double(config_.sample_rate) * 1e9;
    static float max_val_ = 0;
    static bool active_search = false;
    static ConvolutionLatencyEstimator::Timestamp tmp_res;
    ConvolutionLatencyEstimator::Timestamp res;

    optimal_filter_.perform(from, to, sz);
    // Abs( conv_out)
    std::transform(to, to + sz, to, [](const float x) { return fabs(x); });

    for (size_t i = 0; i < sz; ++i) {
        const double cur_ts = (buff_begin_ts_.hw + i2ns(i,config_));
        if(skip_until_ts > cur_ts){
            hw_search_start_ = skip_until_ts;
            active_search = false;
        } else if((hw_search_start_ + hw_search_len_) > cur_ts) {
            active_search = true;

            const float movmax = mmax_.add(to[i]);
            if(movmax > max_val_){
                max_val_ = movmax;
                const double idx_2_ns = i2ns(i, config_);
                tmp_res.samples = buff_begin_ts_.samples + i;
                tmp_res.sw_hw = buff_begin_ts_.sw_hw + idx_2_ns;
                tmp_res.hw = buff_begin_ts_.hw + idx_2_ns;
            }
        } else if(active_search){
            active_search = false;
            max_val_ = 0;
            res = tmp_res;
            return res;
        }
    }

    return res;
}

ConvolutionLatencyEstimator::Timestamp ConvolutionLatencyEstimator::Processor::seek_max(
    float* from, float* to, const size_t sz, double skip_until_ts) {

    static bool max_timeout_ = false; // Start warm-up delay.
    static const size_t timeout_ = buff_len_ * 1.5;
    static size_t last_max_ = 0;

    ConvolutionLatencyEstimator::Timestamp res;
    for (size_t i = 0; i < sz; ++i) {
        if (!max_timeout_) {
            if (fabs(from[i]) > 1e-5 && skip_until_ts < (buff_begin_ts_.hw + i2ns(i, config_))) {
                const double idx_2_ns = i2ns(i, config_);
                last_max_ = 0;
                max_timeout_ = true;
                res.samples = buff_begin_ts_.samples + i;
                res.sw_hw = buff_begin_ts_.sw_hw + idx_2_ns;
                res.hw = buff_begin_ts_.hw + idx_2_ns;
                to[i] = 1.f;
            } else {
                to[i] = 0.f;
            }
        } else {
            to[i] = 0.f;
            last_max_++;
            if (last_max_ > timeout_) {
                max_timeout_ = false;
            }
        }
    }
    return res;
}

ConvolutionLatencyEstimator::Timestamp ConvolutionLatencyEstimator::Processor::update_ts(
    const Frame& frame, size_t idx) const {
    assert(intra_buff_counter_ >= idx);
    Timestamp frame_ts;

    const double idx_2_ns = i2ns(idx, config_);

    frame_ts.samples = idx;
    frame_ts.sw_hw = (double)frame.sw_frame_time() - idx_2_ns;
    frame_ts.hw =    (double)frame.hw_sample_time(0) - idx_2_ns;

    return frame_ts;
}

} // namespace signal_estimator
