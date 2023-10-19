// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Sample.hpp"
#include "core/Time.hpp"

#include <intrusive_shared_ptr/intrusive_shared_ptr.h>

#include <atomic>
#include <cassert>
#include <cstdlib>
#include <vector>

namespace signal_estimator {

class FramePool;
class FrameTraits;

// Frame type
enum class FrameType {
    Output,
    Input,
};

// Audio frame
class Frame {
public:
    Frame(const Config&, FramePool&);

    Frame(const Frame&) = delete;
    Frame& operator=(const Frame&) = delete;

    // reset frame to initial state
    void clear();

    // frame data and size
    size_t size() const;
    const sample_t* data() const;
    sample_t* data();

    // frame type (input or output)
    FrameType type() const;
    void set_type(FrameType);

    // record current time as the time when frame was passed to or retrieved from OS
    void set_time();

    // get time when the frame was passed to or received from the OS
    nanoseconds_t sw_frame_time() const;

    // get time when the frame was passed to or received from hardware
    // this method takes into account OS buffer size
    nanoseconds_t hw_frame_time() const;

    // get time when the sample inside frame was passed to or received from hardware
    // this method takes into account OS buffer size and position of sample in frame
    nanoseconds_t hw_sample_time(size_t sample_index) const;

    // index access
    const sample_t& operator[](const size_t index) const {
        assert(index < data_.size());
        return data_[index];
    }
    sample_t& operator[](const size_t index) {
        assert(index < data_.size());
        return data_[index];
    }

private:
    friend class FramePool;
    friend class FrameTraits;

    int get_ref() const;
    void add_ref();
    void sub_ref();

    const Config& config_;
    FramePool& pool_;

    FrameType io_type_;
    nanoseconds_t io_time_;

    std::vector<sample_t> data_;
    std::atomic<int> refcount_ { 0 };
};

// For FramePtr
class FrameTraits {
public:
    static void add_ref(Frame* frame) noexcept {
        frame->add_ref();
    }

    static void sub_ref(Frame* frame) noexcept {
        frame->sub_ref();
    }
};

// Intrusive shared pointer to frame
using FramePtr = isptr::intrusive_shared_ptr<Frame, FrameTraits>;

} // namespace signal_estimator
