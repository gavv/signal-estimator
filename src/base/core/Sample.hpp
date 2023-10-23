// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <cstdint>

namespace signal_estimator {

using sample_t = int16_t;

constexpr sample_t MaxSample = +32767;
constexpr sample_t MinSample = -32768;

// add or subtract sample_t, saturate on overflow
sample_t saturated_add(sample_t a, sample_t b);
sample_t saturated_sub(sample_t a, sample_t b);

// cast float to sample_t, saturate on overflow
sample_t saturated_cast(float x);
sample_t saturated_cast(double x);

} // namespace signal_estimator
