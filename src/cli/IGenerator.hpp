/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Frame.hpp"

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

// generate output signal
class IGenerator {
public:
    virtual ~IGenerator() = default;

    virtual void generate(Frame& frame) = 0;
};

} // namespace signal_estimator
