// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once
#include "processing/IEstimatorWrapper.hpp"
#include "core/Config.hpp"


namespace signal_estimator {

// estimate and report some signal characteristic
class MinMaxEstimator :public IEstimatorWrapper {
public:
    MinMaxEstimator(const Config& config, std::unique_ptr<IEstimator> estimator) 
    : IEstimatorWrapper(std::move(estimator)),
    config_(config){}
    // called from output thread
    void add_output(FramePtr frame) override;

    // called from input thread
    void add_input(FramePtr frame) override;
private:
    std::atomic<bool> impulse_started_{};
    std::atomic<nanoseconds_t> impulse_time_{};
    const Config config_;
};

} // namespace signal_estimator