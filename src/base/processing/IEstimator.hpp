// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Frame.hpp"

#include <cstdint>
#include <cstdlib>
#include <memory>

namespace signal_estimator {

// estimate and report some signal characteristic
class IEstimator {
public:
    virtual ~IEstimator() = default;

    // called from output thread
    virtual void add_output(std::shared_ptr<Frame> frame) = 0;

    // called from input thread
    virtual void add_input(std::shared_ptr<Frame> frame) = 0;
};

} // namespace signal_estimator
