/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "Time.hpp"

#include <chrono>

namespace signal_estimator {

namespace {

unsigned long get_ts() {
    return (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch())
                               .count());
}

} // namespace

unsigned long monotonic_timestamp() {
    static unsigned long start_ts = get_ts();
    return get_ts() - start_ts;
}

} // namespace signal_estimator
