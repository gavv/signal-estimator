// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Frame.hpp"
#include "core/Queue.hpp"
#include "processing/FFTConvolution.hpp"
#include "processing/IEstimator.hpp"
#include "processing/Impulse.hpp"
#include "processing/MovAvg.hpp"
#include "processing/MovMax.hpp"
#include "reports/IReporter.hpp"

#include <string>
#include <thread>
#include <vector>

namespace signal_estimator {

class CorrelationLatencyEstimator : public IEstimator {
public:
    CorrelationLatencyEstimator(const Config& config, IReporter& reporter);
    ~CorrelationLatencyEstimator();

    CorrelationLatencyEstimator(const CorrelationLatencyEstimator&) = delete;
    CorrelationLatencyEstimator& operator=(const CorrelationLatencyEstimator&) = delete;

    void add_output(FramePtr frame) override;
    void add_input(FramePtr frame) override;

private:
    struct Timestamp {
        double sw_hw = 0;
        double hw = 0;

        explicit operator bool() const {
            return sw_hw > 0 || hw > 0;
        }
    };

    class Processor {
    public:
        Processor(const Config& config);

        Timestamp operator()(
            const Frame& frame, const bool plain_simple, double skip_until_ts);

    private:
        Timestamp seek_correlation_(
            const float* from, float* to, const size_t sz, double skip_until_ts);
        Timestamp seek_max_(
            const float* from, float* to, const size_t sz, double skip_until_ts);

        // Return what time stamp was frame_offset samples before frame's timestamp.
        // frame_offset              Frame
        //  |.......................|*********|
        Timestamp compute_ts_(const Frame& frame, size_t frame_offset) const;

        const Config& config_;

        static constexpr size_t buff_len_ = impulse.size();

        FFTConvolution<buff_len_, impulse.size(), impulse> optimal_filter_;
        MovMax<float, false> mmax_;
        MovAvg<float> mmavg_;

        std::array<float, buff_len_> buff_;
        Timestamp buff_begin_ts_;

        size_t inter_buff_i_ = 0;
        size_t intra_buff_counter_ = 0;

        double hw_search_start_ = 0;
        double hw_search_len_ = 0;
        bool hw_search_activated_ = false;

        Timestamp max_corr_ts_;
        float max_corr_val_ = 0;

        bool max_timeout_activated_ = false;
        size_t max_timeout_counter_ = 0;
    };

    void run_();
    void report_(Timestamp out_peak, Timestamp in_peak);

    const Config& config_;
    IReporter& reporter_;
    MovAvg<double> hw_avg_;
    Queue<FramePtr> queue_in_;
    Queue<FramePtr> queue_out_;
    std::thread thread_;

    const double causality_timeout_lim_;
    double causality_timeout_counter_ = 0;

    Processor in_processor_, out_processor_;
};

} // namespace signal_estimator
