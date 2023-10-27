// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/Time.hpp"

#include <chrono>

namespace signal_estimator {

nanoseconds_t monotonic_timestamp_ns() {
    return nanoseconds_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch())
                             .count());
}

nanoseconds_t wallclock_timestamp_ns() {
    return nanoseconds_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch())
                             .count());
}

} // namespace signal_estimatorc
