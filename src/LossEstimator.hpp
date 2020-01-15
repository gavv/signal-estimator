/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"
#include "GradientCounter.hpp"
#include "IEstimator.hpp"
#include "RateLimiter.hpp"
#include "RunMaxCounter.hpp"
#include "SchmittTrigger.hpp"
#include "SmaCounter.hpp"

namespace signal_estimator {

// estimate signal loss ratio
// assumes that the output signal is a smooth sine wave produced by ContinuousGenerator
// detects spikes in the output signal
class LossEstimator : public IEstimator {
public:
    LossEstimator(const Config& config);

    LossEstimator(const LossEstimator&) = delete;
    LossEstimator& operator=(const LossEstimator&) = delete;

    void add_output(Frame& frame) override;
    void add_input(Frame& frame) override;

private:
    void report_losses_();

    const Config config_;

    GradientCounter gradient_;
    RunMaxCounter runmax_;
    SchmittTrigger schmitt_;

    size_t losses_ {};
    RateLimiter limiter_ { 2 };
    SmaCounter sma_;
};

} // namespace signal_estimator
