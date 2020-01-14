/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include <vector>

namespace signal_estimator {

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
