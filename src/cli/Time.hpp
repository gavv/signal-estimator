/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"

namespace signal_estimator {

using nanoseconds_t = long;

// current timestamp from monotonic source
// starts from zero (returned on the first invocation)
nanoseconds_t monotonic_timestamp_ns();

} // namespace signal_estimator
