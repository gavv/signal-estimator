// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/RateLimiter.hpp"
#include "processing/IEstimator.hpp"
#include "processing/MovAvg.hpp"
#include "processing/MovGradient.hpp"
#include "processing/MovMax.hpp"
#include "processing/SchmittTrigger.hpp"
#include "reports/IReporter.hpp"

#include <memory>

namespace signal_estimator {

// estimate signal loss ratio
// assumes that the output signal is a smooth sine wave produced by ContinuousGenerator
// detects spikes in the output signal
class LossEstimator : public IEstimator {
public:
    LossEstimator(const Config& config, IReporter& reporter);

    LossEstimator(const LossEstimator&) = delete;
    LossEstimator& operator=(const LossEstimator&) = delete;

    ~LossEstimator() override;

    void add_output(FramePtr frame) override;
    void add_input(FramePtr frame) override;

private:
    void process_frame_(const Frame& frame);
    void report_losses_();

    const Config config_;

    MovMax<float> signal_runmax_;

    MovGradient<float> gradient_;
    MovMax<float> gradient_runmax_;
    SchmittTrigger<float> gradient_schmitt_;

    bool leading_zeros_ { true };
    uint64_t signal_ {};
    uint64_t no_signal_ {};

    uint64_t losses_ {};

    RateLimiter limiter_ { 2 };
    MovAvg<double> sma_;

    IReporter& reporter_;
};

} // namespace signal_estimator
