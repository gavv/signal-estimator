// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/FrameQueue.hpp"
#include "core/RateLimiter.hpp"
#include "core/Time.hpp"
#include "processing/IEstimator.hpp"
#include "processing/MovAvg.hpp"
#include "processing/MovPercentile.hpp"
#include "reports/IReporter.hpp"

#include <memory>
#include <thread>

namespace signal_estimator {

// calculates jitter of frame timestamps
class IODelayEstimator : public IEstimator {
public:
    IODelayEstimator(const Config& config, const DevInfo& dev_info, IReporter& reporter);
    ~IODelayEstimator() override;

    IODelayEstimator(const IODelayEstimator&) = delete;
    IODelayEstimator& operator=(const IODelayEstimator&) = delete;

    void add_output(FramePtr frame) override;
    void add_input(FramePtr frame) override;

private:
    void run_();

    const Config config_;
    const DevInfo dev_info_;

    FrameQueue queue_;
    std::thread thread_;

    MovAvg<double> sw_delay_avg_;
    MovAvg<double> hw_delay_avg_;

    RateLimiter report_limiter_ { 2 };

    IReporter& reporter_;
};

} // namespace signal_estimator
