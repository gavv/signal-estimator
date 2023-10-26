// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/FrameQueue.hpp"

namespace signal_estimator {

bool FrameQueue::empty() const {
    return size_ == 0;
}

void FrameQueue::push(FramePtr frame) {
    queue_.enqueue(std::move(frame));
    size_++;
}

FramePtr FrameQueue::try_pop() {
    FramePtr frame;
    if (!queue_.try_dequeue(frame)) {
        return {};
    }
    size_--;
    return frame;
}

FramePtr FrameQueue::wait_pop() {
    FramePtr frame;
    queue_.wait_dequeue(frame);
    size_--;
    return frame;
}

} // namespace signal_estimator
