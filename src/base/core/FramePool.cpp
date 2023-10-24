// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/FramePool.hpp"

namespace signal_estimator {

FramePool::FramePool(const Config& config)
    : config_(config) {
    for (size_t n = 0; n < config.frame_pool_size; n++) {
        free_list_.enqueue(create_frame_());
    }
}

FramePool::~FramePool() {
    Frame* frame = nullptr;

    while (free_list_.try_dequeue(frame)) {
        delete_frame_(frame);
    }
}

FramePtr FramePool::allocate(Dir dir, size_t dev_index) {
    Frame* frame = nullptr;

    if (!free_list_.try_dequeue(frame)) {
        frame = create_frame_();
    }

    frame->reset(dir, dev_index);

    assert(frame->get_ref() == 0);

    return FramePtr::ref(frame);
}

void FramePool::release_frame_(Frame* frame) {
    assert(frame->get_ref() == 0);

    free_list_.enqueue(frame);
}

Frame* FramePool::create_frame_() {
    return new Frame(config_, *this);
}

void FramePool::delete_frame_(Frame* frame) {
    assert(frame->get_ref() == 0);

    delete frame;
}

} // namespace signal_estimator
