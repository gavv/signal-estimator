// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Frame.hpp"

#include <blockingconcurrentqueue.h>

#include <atomic>

namespace signal_estimator {

// thread-safe lock-free frame queue
class FrameQueue {
public:
    FrameQueue() = default;

    FrameQueue(const FrameQueue&) = delete;
    FrameQueue& operator=(const FrameQueue&) = delete;

    // check if queue is empty
    bool empty() const;

    // lock-free operation
    void push(FramePtr frame);

    // returns null if queue is empty or if null frame was pushed
    FramePtr try_pop();

    // returns null only if null frame was pushed
    FramePtr wait_pop();

private:
    moodycamel::BlockingConcurrentQueue<FramePtr> queue_;
    std::atomic<size_t> size_ { 0 };
};

} // namespace signal_estimator
