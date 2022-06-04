// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <cstdlib>
#include <vector>

namespace signal_estimator {

// calculate gradient using central difference
template <typename T = double> class MovGradient {
public:
    MovGradient() = default;

    T operator()(const T x) {
        add(x);
        return get();
    }

    void add(const T val) {
        if (size_ < 2) {
            prev_[size_++] = val;
        } else {
            grad_ = (val - prev_[0]) / 2;

            prev_[0] = prev_[1];
            prev_[1] = val;
        }
    }

    T get() const {
        return grad_;
    }

private:
    size_t size_ {};
    T prev_[2] {};
    T grad_ { 0 };
};

} // namespace signal_estimator
