// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Frame.hpp"

#include <memory>

#include <concurrentqueue.h>

namespace signal_estimator {

class FramePool {
public:
    FramePool(const Config& config);
    ~FramePool();

    FramePool(const FramePool&) = delete;
    FramePool& operator=(const FramePool&) = delete;

    std::shared_ptr<Frame> allocate();

private:
    void release_frame_(Frame*);

    Frame* create_frame_();
    void delete_frame_(Frame* frame);

    moodycamel::ConcurrentQueue<Frame*> free_list_;
    const Config config_;
};

} // namespace signal_estimator
