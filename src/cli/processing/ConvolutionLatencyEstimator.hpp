// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Frame.hpp"
#include "core/Queue.hpp"
#include "fmt/IFormatter.hpp"
#include "processing/FFTConvolution.hpp"
#include "processing/IEstimator.hpp"
#include "processing/Impulse.hpp"
#include "processing/MovAvg.hpp"
#include "processing/MovMax.hpp"

#include <string>
#include <thread>
#include <vector>

namespace signal_estimator {

class ConvolutionLatencyEstimator : public IEstimator {
public:
    ConvolutionLatencyEstimator(const Config& config, IFormatter& formatter);
    ~ConvolutionLatencyEstimator();

    ConvolutionLatencyEstimator(const ConvolutionLatencyEstimator&) = delete;
    ConvolutionLatencyEstimator& operator=(const ConvolutionLatencyEstimator&) = delete;

    void add_output(std::shared_ptr<Frame> frame) override;
    void add_input(std::shared_ptr<Frame> frame) override;

private:
    void run();

    const Config& config_;
    IFormatter& formatter_;
    MovAvg<double> hw_avg_;
    Queue<std::shared_ptr<Frame>> queue_in_;
    Queue<std::shared_ptr<Frame>> queue_out_;
    std::thread thread_;

    const double causality_timeout_lim_;
    double causality_timeoute_counter_ = 0;

    struct Timestamp {
        size_t samples = 0;
        double sw_hw = 0;
        double hw = 0;

        Timestamp() = default;

        constexpr Timestamp& operator=(const Timestamp&) = default;

        explicit operator bool() const {
            return this->samples > 0;
        }

        Timestamp operator-(const Timestamp& x) const {
            Timestamp res;
            res.samples = samples - x.samples;
            res.sw_hw = sw_hw - x.sw_hw;
            res.hw = hw - x.hw;
            return res;
        }
    };

    class Processor {
    public:
        Processor(const Config& config);

        Timestamp operator()(
            Frame&, const bool plain_simple = false, double skip_until_ts = 0);

    private:
        Timestamp process_buff(
            const float* from, float* to, const size_t sz, double skip_until_ts = 0);
        Timestamp seek_max(
            float* from, float* to, const size_t sz, double skip_until_ts = 0);
        // Return what time stamp was idx samples before frame's timestamp.
        // ts                          Frame
        //  |.......................|*********|
        Timestamp update_ts(const Frame& frame, size_t idx) const;

        const Config& config_;
        static constexpr size_t buff_len_ = impulse.size();
        FFTConvolution<buff_len_, impulse.size(), impulse> optimal_filter_;
        MovMax<float, false> mmax_;

        std::array<float, buff_len_> buff_;
        Timestamp buff_begin_ts_;
        size_t inter_buff_i_;
        size_t intra_buff_counter_;
    } in_processor_, out_processor_;
};

} // namespace signal_estimator
