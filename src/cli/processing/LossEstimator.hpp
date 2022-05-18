// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "io/IFormatter.hpp"
#include "processing/GradientCounter.hpp"
#include "processing/IEstimator.hpp"
#include "processing/MovAvg.hpp"
#include "processing/MovMax.hpp"
#include "processing/RateLimiter.hpp"
#include "processing/SchmittTrigger.hpp"

#include <memory>

namespace signal_estimator {

// estimate signal loss ratio
// assumes that the output signal is a smooth sine wave produced by ContinuousGenerator
// detects spikes in the output signal
class LossEstimator : public IEstimator {
public:
    LossEstimator(const Config& config, IFormatter& formatter);

    LossEstimator(const LossEstimator&) = delete;
    LossEstimator& operator=(const LossEstimator&) = delete;

    ~LossEstimator() override;

    void add_output(std::shared_ptr<Frame>) override;
    void add_input(std::shared_ptr<Frame> frame) override;

private:
    void report_losses_();

    const Config config_;

    MovMax<double> signal_runmax_;

    GradientCounter gradient_;
    MovMax<double> gradient_runmax_;
    SchmittTrigger gradient_schmitt_;

    bool leading_zeros_ { true };
    uint64_t signal_ {};
    uint64_t no_signal_ {};

    uint64_t losses_ {};

    RateLimiter limiter_ { 2 };
    MovAvg<double> sma_;

    IFormatter& format_;
};

} // namespace signal_estimator
