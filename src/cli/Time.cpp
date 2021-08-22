// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "Time.hpp"

#include <chrono>

namespace signal_estimator {

namespace {

nanoseconds_t get_ts() {
    return (nanoseconds_t)(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch())
                               .count());
}

} // namespace

nanoseconds_t monotonic_timestamp_ns() {
    static nanoseconds_t start_ts = get_ts();
    return get_ts() - start_ts;
}

} // namespace signal_estimatorc
