// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <algorithm>
#include <vector>

namespace signal_estimator {

// moving percentile
// calculates lowest value that is larger than P% of values in moving window
// based on https://stackoverflow.com/questions/3738349
template <typename T = double> class MovPercentile {
public:
    // percentile should be in range [0; 1]
    MovPercentile(const size_t window_sz, const double percentile)
        : window_sz_(window_sz)
        , percentile_(percentile)
        , buff_(window_sz)
        , start_(0)
        , end_(0) {
    }

    T operator()(const T x) {
        add(x);
        return get();
    }

    void add(const T x) {
        buff_[end_ % window_sz_] = x;
        end_++;

        if (end_ - start_ > window_sz_) {
            start_++;
        }

        clear_();

        // N * log(N)
        for (size_t i = start_; i != end_; i++) {
            push_(buff_[i % window_sz_]);
        }
    }

    T get() const {
        return lower_.front();
    }

    bool is_full() const {
        return end_ - start_ == window_sz_;
    }

private:
    void push_(T x) {
        if (lower_.empty() || x <= lower_.front()) {
            lower_.push_back(x);
            std::push_heap(lower_.begin(), lower_.end(), std::less<T>());
        } else {
            upper_.push_back(x);
            std::push_heap(upper_.begin(), upper_.end(), std::greater<T>());
        }

        const size_t size_lower
            = size_t((lower_.size() + upper_.size()) * percentile_ + 1);

        if (lower_.size() > size_lower) {
            // lower to upper
            if (!lower_.empty()) {
                std::pop_heap(lower_.begin(), lower_.end(), std::less<T>());
                upper_.push_back(lower_.back());
                std::push_heap(upper_.begin(), upper_.end(), std::greater<T>());
                lower_.pop_back();
            }
        } else if (lower_.size() < size_lower) {
            // upper to lower
            if (!upper_.empty()) {
                std::pop_heap(upper_.begin(), upper_.end(), std::greater<T>());
                lower_.push_back(upper_.back());
                std::push_heap(lower_.begin(), lower_.end(), std::less<T>());
                upper_.pop_back();
            }
        }
    }

    void clear_() {
        lower_.clear();
        upper_.clear();
    }

    const size_t window_sz_;
    const double percentile_;

    std::vector<T> buff_;
    size_t start_;
    size_t end_;

    std::vector<T> lower_;
    std::vector<T> upper_;
};

} // signal_estimator
