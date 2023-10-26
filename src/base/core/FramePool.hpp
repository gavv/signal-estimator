// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Frame.hpp"

#include <memory>

#include <concurrentqueue.h>

namespace signal_estimator {

// thread-safe lock-free frame pool
class FramePool {
public:
    FramePool(const Config& config);
    ~FramePool();

    FramePool(const FramePool&) = delete;
    FramePool& operator=(const FramePool&) = delete;

    // allocate and reset() frame
    // frame smart pointer will automatically return frame to pool
    FramePtr allocate(Dir dir, size_t dev_index);

private:
    friend class Frame;

    void release_frame_(Frame*);

    Frame* create_frame_();
    void delete_frame_(Frame* frame);

    moodycamel::ConcurrentQueue<Frame*> free_list_;
    const Config config_;
};

} // namespace signal_estimator
