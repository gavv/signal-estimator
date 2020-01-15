/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include <cstdlib>

namespace signal_estimator {

class SchmittTrigger {
public:
    SchmittTrigger(double threshold);

    SchmittTrigger(const SchmittTrigger&) = delete;
    SchmittTrigger& operator=(const SchmittTrigger&) = delete;

    bool add(double);

private:
    const double lower_threshold_;
    const double upper_threshold_;

    bool state_ {};
};

} // namespace signal_estimator
