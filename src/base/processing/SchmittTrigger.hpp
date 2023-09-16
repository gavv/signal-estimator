// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Sample.hpp"

namespace signal_estimator {

// software schmitt trigger (a comparator with hysteresis)
template <class T = double> class SchmittTrigger {
public:
    SchmittTrigger(T threshold)
        : lower_threshold_(T(MaxSample) * threshold / 2)
        , upper_threshold_(T(MaxSample) * threshold) {
    }

    bool operator()(const T val) {
        const bool old_state = get();
        add(val);
        const bool new_state = get();

        return !old_state && new_state;
    }

    void add(const T val) {
        if (state_) {
            if (val < lower_threshold_) {
                state_ = false;
            }
        } else {
            if (val > upper_threshold_) {
                state_ = true;
            }
        }
    }

    bool get() const {
        return state_;
    }

private:
    const T lower_threshold_;
    const T upper_threshold_;

    bool state_ { false };
};

} // namespace signal_estimator
