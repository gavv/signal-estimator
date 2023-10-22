// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Queue.hpp"
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
class IOJitterEstimator : public IEstimator {
public:
    IOJitterEstimator(const Config& config, Dir dir, IReporter& reporter);
    ~IOJitterEstimator() override;

    IOJitterEstimator(const IOJitterEstimator&) = delete;
    IOJitterEstimator& operator=(const IOJitterEstimator&) = delete;

    void add_output(FramePtr frame) override;
    void add_input(FramePtr frame) override;

private:
    void run_();

    struct JitterStats {
        // previous frame time
        nanoseconds_t prev_ts { 0 };
        // average deviation of frame time delta from ideal period
        MovAvg<double> dev_avg;
        // Nth percentile of deviation of frame time delta from ideal period
        MovPercentile<double> dev_per;
        // ideal period
        const double ideal_period;

        JitterStats(const Config& config, Dir dir);

        void update(nanoseconds_t next_ts);
    };

    struct BufStats {
        // average hw buffer length
        MovAvg<double> len_avg;
        // Nth percentile of hw buffer length
        MovPercentile<double> len_per;

        BufStats(const Config& config);

        void update(nanoseconds_t buf_len);
    };

    const Config config_;
    const Dir dir_;

    Queue<FramePtr> queue_;
    std::thread thread_;

    JitterStats sw_stats_;
    JitterStats hw_stats_;
    BufStats buf_stats_;

    RateLimiter report_limiter_ { 2 };

    IReporter& reporter_;
};

} // namespace signal_estimator
