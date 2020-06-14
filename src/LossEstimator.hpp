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
#include "IFormatter.hpp"

namespace signal_estimator {

// estimate signal loss ratio
// assumes that the output signal is a smooth sine wave produced by ContinuousGenerator
// detects spikes in the output signal
class LossEstimator : public IEstimator {
public:
    LossEstimator(const Config& config);

    LossEstimator(const LossEstimator&) = delete;
    LossEstimator& operator=(const LossEstimator&) = delete;

	~LossEstimator();

    void add_output(Frame& frame) override;
    void add_input(Frame& frame) override;

private:
    void report_losses_();

    const Config config_;

    RunMaxCounter signal_runmax_;

    GradientCounter gradient_;
    RunMaxCounter gradient_runmax_;
    SchmittTrigger gradient_schmitt_;

    bool leading_zeros_ { true };
    uint64_t signal_ {};
    uint64_t no_signal_ {};

    uint64_t losses_ {};

    RateLimiter limiter_ { 2 };
    SmaCounter sma_;

    IFormatter* format;
};

} // namespace signal_estimator
