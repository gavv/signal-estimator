// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <algorithm>
#include <vector>

namespace signal_estimator {

// moving average (SMA)
template <typename T = double> class MovAvg {
public:
    explicit MovAvg(const size_t window_sz)
        : window_sz_(window_sz)
        , buff_(window_sz)
        , init_counter_(0)
        , cur_i_(0)
        , accum_(0) {
    }

    T operator()(const T x) {
        add(x);
        return get();
    }

    void add(const T x) {
        accum_ -= buff_[cur_i_];
        accum_ += x;
        buff_[cur_i_] = x;
        if (++cur_i_ >= window_sz_) {
            cur_i_ = 0;
        }
        if (init_counter_ < window_sz_) {
            init_counter_++;
        }
    }

    T get() const {
        return accum_ / T(init_counter_);
    }

    bool is_full() const {
        return init_counter_ == window_sz_;
    }

private:
    const size_t window_sz_;
    std::vector<T> buff_;
    size_t init_counter_;
    size_t cur_i_;
    T accum_;
};

} // signal_estimator
