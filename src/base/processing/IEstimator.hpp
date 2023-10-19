// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Frame.hpp"

namespace signal_estimator {

// estimate and report some signal characteristic
class IEstimator {
public:
    virtual ~IEstimator() = default;

    // called from output thread
    virtual void add_output(FramePtr frame) = 0;

    // called from input thread
    virtual void add_input(FramePtr frame) = 0;
};

} // namespace signal_estimator
