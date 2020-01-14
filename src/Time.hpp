/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"
#include "Direction.hpp"

#include <cstdint>

namespace signal_estimator {

using nanoseconds_t = long;

// current timestamp
nanoseconds_t monotonic_timestamp_ns();

// calculate time offset in nanoseconds when the sample will be actually played
// or was actually recorded
nanoseconds_t sample_offset(const Config& config, Dir direction, size_t frame_off);

} // namespace signal_estimator
