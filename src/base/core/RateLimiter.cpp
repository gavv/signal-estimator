// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/RateLimiter.hpp"

namespace signal_estimator {

RateLimiter::RateLimiter(float period_sec)
    : allow_period_(nanoseconds_t(period_sec * 1000000000)) {
}

double RateLimiter::allow() {
    const nanoseconds_t cur_pos = elapsed_nsec_();

    if (cur_pos <= next_allow_pos_) {
        return 0;
    }

    const double ret = (cur_pos - last_allow_pos_) / 1000000000.0;

    next_allow_pos_ = (cur_pos / allow_period_ + 1) * allow_period_;
    last_allow_pos_ = cur_pos;

    return ret;
}

nanoseconds_t RateLimiter::elapsed_nsec_() {
    if (!started_) {
        start_pos_ = monotonic_timestamp_ns();
        started_ = true;
        return 0;
    } else {
        return monotonic_timestamp_ns() - start_pos_;
    }
}

} // namespace signal_estimator
