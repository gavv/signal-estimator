// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/Sample.hpp"

namespace signal_estimator {

sample_t saturated_add(sample_t a, sample_t b) {
    if (a > 0) {
        if (b > MaxSample - a) {
            return MaxSample;
        }
    } else {
        if (b < MinSample - a) {
            return MinSample;
        }
    }

    return a + b;
}

sample_t saturated_sub(sample_t a, sample_t b) {
    if (a > 0) {
        if (b < a - MaxSample) {
            return MaxSample;
        }
    } else {
        if (b > a - MinSample) {
            return MinSample;
        }
    }

    return a - b;
}

sample_t saturated_cast(float x) {
    if (x > MaxSample) {
        return MaxSample;
    } else if (x < MinSample) {
        return MinSample;
    }

    return (sample_t)x;
}

sample_t saturated_cast(double x) {
    if (x > MaxSample) {
        return MaxSample;
    } else if (x < MinSample) {
        return MinSample;
    }

    return (sample_t)x;
}

} // namespace signal_estimator
