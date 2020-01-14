/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "SchmittTrigger.hpp"
#include "Frame.hpp"

#include <algorithm>

namespace signal_estimator {

SchmittTrigger::SchmittTrigger(double threshold)
    : lower_threshold_(MaxSample * threshold / 2)
    , upper_threshold_(MaxSample * threshold) {
}

bool SchmittTrigger::add(double s) {
    if (state_) {
        if (s < lower_threshold_) {
            state_ = false;
        }
    } else {
        if (s > upper_threshold_) {
            state_ = true;
            return true;
        }
    }

    return false;
}

} // namespace signal_estimator
