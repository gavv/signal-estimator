/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Frame.hpp"

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

// estimate and report some signal characteristic
class IEstimator {
public:
    virtual ~IEstimator() = default;

    // called from output thread
    virtual void add_output(Frame& frame) = 0;

    // called from input thread
    virtual void add_input(Frame& frame) = 0;
};

} // namespace signal_estimator
