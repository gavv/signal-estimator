// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <atomic>

#include <blockingconcurrentqueue.h>

namespace signal_estimator {

template <typename T> class Queue {
public:
    Queue() = default;

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    bool empty() const {
        return size_ == 0;
    }

    void push(const T val) {
        queue_.enqueue(val);
        size_++;
    }

    T try_pop() {
        T val;
        if (!queue_.try_dequeue(val)) {
            return {};
        }
        size_--;
        return val;
    }

    T wait_pop() {
        T val;
        queue_.wait_dequeue(val);
        size_--;
        return val;
    }

private:
    moodycamel::BlockingConcurrentQueue<T> queue_;
    std::atomic<size_t> size_ { 0 };
};

} // namespace signal_estimator
