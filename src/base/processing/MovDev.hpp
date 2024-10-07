// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "processing/MovAvg.hpp"

#include <algorithm>
#include <vector>

namespace signal_estimator {

// moving deviation
// calculates average and maximum absolute deviations from average value
// accross moving window
template <typename T = double> class MovDev {
public:
    explicit MovDev(const size_t window_sz)
        : window_sz_(window_sz)
        , mean_(window_sz)
        , buff_(window_sz)
        , start_(0)
        , end_(0) {
    }

    void add(const T x) {
        mean_.add(x);

        buff_[end_ % window_sz_] = x;
        end_++;

        if (end_ - start_ > window_sz_) {
            start_++;
        }
    }

    // average deviation from arithmetic mean
    T avg() const {
        if (end_ == start_) {
            return 0;
        }

        const T mean = mean_.get();

        T ret = 0;
        for (size_t i = start_; i != end_; i++) {
            ret += std::abs(buff_[i % window_sz_] - mean);
        }
        ret /= T(end_ - start_);

        return ret;
    }

    // maximum deviation from arithmetic mean
    T max() const {
        if (end_ == start_) {
            return 0;
        }

        const T mean = mean_.get();

        T ret = 0;
        for (size_t i = start_; i != end_; i++) {
            ret = std::max(ret, std::abs(buff_[i % window_sz_] - mean));
        }

        return ret;
    }

    bool is_full() const {
        return end_ - start_ == window_sz_;
    }

private:
    const size_t window_sz_;
    MovAvg<T> mean_;

    std::vector<T> buff_;
    size_t start_;
    size_t end_;
};

} // signal_estimator
