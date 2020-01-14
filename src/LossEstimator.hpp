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

class LossEstimator : public IEstimator {
public:
    LossEstimator(const Config& config);

    LossEstimator(const LossEstimator&) = delete;
    LossEstimator& operator=(const LossEstimator&) = delete;

    void add_output(nanoseconds_t ts, const int16_t* buf, size_t bufsz) override;
    void add_input(nanoseconds_t ts, const int16_t* buf, size_t bufsz) override;

private:
    const Config config_;

    GradientCounter gradient_;
    RunMaxCounter runmax_;
    SchmittTrigger schmitt_;

    size_t losses_ {};
    RateLimiter limiter_ { 2 };
    SmaCounter sma_;
};

} // namespace signal_estimator
