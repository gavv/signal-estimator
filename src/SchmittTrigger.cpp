/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "SchmittTrigger.hpp"
#include "Time.hpp"

#include <algorithm>

namespace signal_estimator {

SchmittTrigger::SchmittTrigger(const Config& config)
    : config_(config)
    , lower_threshold_(32767 * config.strike_threshold / 2)
    , upper_threshold_(32767 * config.strike_threshold)
    , runmax_(config.strike_window) {
}

void SchmittTrigger::add_signal(
    nanoseconds_t ts, Dir dir, const int16_t* buf, size_t bufsz) {
    for (size_t n = 0; n < bufsz; n++) {
        const auto s = runmax_.add(double(std::abs(buf[n])));

        if (state_) {
            if (s < lower_threshold_) {
                state_ = false;
            }
        } else {
            if (s > upper_threshold_) {
                state_ = true;
                trigger_ts_msec_
                    = double(ts + sample_offset(config_, dir, n)) / 1000000.0;
            }
        }
    }
}

} // namespace signal_estimator
