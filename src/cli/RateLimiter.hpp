// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "Time.hpp"

#include <cstdint>

namespace signal_estimator {

// limit events to given rate per second
class RateLimiter {
public:
    explicit RateLimiter(float period_sec);

    double allow();

private:
    nanoseconds_t elapsed_nsec_();

    const nanoseconds_t allow_period_;

    nanoseconds_t last_allow_pos_ {};
    nanoseconds_t next_allow_pos_ {};

    nanoseconds_t start_pos_ {};
    nanoseconds_t started_ { false };
};

} // namespace signal_estimator
