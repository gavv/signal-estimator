// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <cstdlib>

namespace signal_estimator {

// software schmitt trigger (a comparator with hysteresis)
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
