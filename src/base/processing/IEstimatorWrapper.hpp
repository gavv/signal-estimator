// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Frame.hpp"
#include "processing/IEstimator.hpp"
#include <memory>

namespace signal_estimator {

// estimate and report some signal characteristic
class IEstimatorWrapper :public IEstimator {
protected:
    std::unique_ptr<IEstimator> m_Estimator;
public:
    IEstimatorWrapper(std::unique_ptr<IEstimator> estimator) : m_Estimator(std::move(estimator)){}
    // called from output thread
    void add_output(FramePtr frame) override {
        m_Estimator->add_output(frame);
    }

    // called from input thread
    void add_input(FramePtr frame) override  {
        m_Estimator->add_input(frame);
    }
};

} // namespace signal_estimator