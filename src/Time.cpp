/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

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

nanoseconds_t sample_offset(const Config& config, Dir direction, size_t frame_off) {
    if (direction == Dir::Playback) {
        return nanoseconds_t((config.n_periods - 1) * config.period_size + frame_off)
            / config.n_channels * 1000000000 / config.sample_rate;
    } else {
        return -nanoseconds_t(config.period_size - frame_off) / config.n_channels
            * 1000000000 / config.sample_rate;
    }
}

} // namespace signal_estimatorc
