/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "SmaCounter.hpp"

namespace signal_estimator {

SmaCounter::SmaCounter(size_t size)
    : window_(size * 2) {
}

double SmaCounter::add(double val) {
    if (size_ < window_.size()) {
        // until ring buffer is full, we report arithmetic mean
        size_++;
        avg_ += (val - avg_) / size_;

        window_[tail_] = val;
        tail_ = (tail_ + 1) % window_.size();

        return avg_;
    } else {
        // when ring buffer is full, we report simple moving average
        avg_ += (val - window_[head_]) / size_;

        window_[tail_] = val;
        tail_ = (tail_ + 1) % size_;
        head_ = (head_ + 1) % size_;
    }

    return avg_;
}

} // namespace signal_estimator
