// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

namespace signal_estimator {

//! Nanoseconds type.
using nanoseconds_t = long long;

//! One nanosecond represented in nanoseconds.
inline constexpr nanoseconds_t Nanosecond = 1;

//! One microsecond represented in nanoseconds.
inline constexpr nanoseconds_t Microsecond = 1000 * Nanosecond;

//! One millisecond represented in nanoseconds.
inline constexpr nanoseconds_t Millisecond = 1000 * Microsecond;

//! One second represented in nanoseconds.
inline constexpr nanoseconds_t Second = 1000 * Millisecond;

//! One minute represented in nanoseconds.
inline constexpr nanoseconds_t Minute = 60 * Second;

//! One hour represented in nanoseconds.
inline constexpr nanoseconds_t Hour = 60 * Minute;

// current timestamp from monotonic source
nanoseconds_t monotonic_timestamp_ns();

// current timestamp from wallclock source
nanoseconds_t wallclock_timestamp_ns();

} // namespace signal_estimator
