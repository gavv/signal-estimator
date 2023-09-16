// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <cstdint>

namespace signal_estimator {

using sample_t = int16_t;

constexpr sample_t MaxSample = +32767;
constexpr sample_t MinSample = -32767;

} // namespace signal_estimator
