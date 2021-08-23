// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <cstdlib>
#include <vector>

namespace signal_estimator {

// calculate gradient using central difference
class GradientCounter {
public:
    GradientCounter() = default;

    GradientCounter(const GradientCounter&) = delete;
    GradientCounter& operator=(const GradientCounter&) = delete;

    double add(double);

private:
    size_t size_ {};
    double prev_[2];
};

} // namespace signal_estimator
