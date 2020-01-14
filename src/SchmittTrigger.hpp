/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"
#include "RunMaxCounter.hpp"
#include "Time.hpp"

#include <cstdint>
#include <cstdlib>

namespace signal_estimator {

class SchmittTrigger {
public:
    SchmittTrigger(const Config& config);

    SchmittTrigger(const SchmittTrigger&) = delete;
    SchmittTrigger& operator=(const SchmittTrigger&) = delete;

    void add_signal(nanoseconds_t ts, Dir direction, const int16_t* buf, size_t bufsz);

    bool is_triggered() const { return state_; }

    double trigger_ts_msec() const { return trigger_ts_msec_; }

private:
    const Config config_;

    double lower_threshold_ {};
    double upper_threshold_ {};

    bool state_ {};
    double trigger_ts_msec_ {};

    RunMaxCounter runmax_;
};

} // namespace signal_estimator
