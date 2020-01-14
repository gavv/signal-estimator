/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Time.hpp"

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

class IEstimator {
public:
    virtual ~IEstimator() = default;

    virtual void add_output(nanoseconds_t ts, const int16_t* buf, size_t bufsz) = 0;
    virtual void add_input(nanoseconds_t ts, const int16_t* buf, size_t bufsz) = 0;
};

} // namespace signal_estimator
