// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Dir.hpp"
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

// Audio frame
class Frame {
public:
    Frame(const Config&, FramePool&);

    Frame(const Frame&) = delete;
    Frame& operator=(const Frame&) = delete;

    // reset frame to initial state
    void reset(Dir dir, size_t dev_index);

    // source or destination device
    const DevInfo& dev() const;

    // frame direction (input or output)
    Dir dir() const;

    // frame size and data
    size_t size() const;
    const sample_t* data() const;
    sample_t* data();

    // record hw and sw times
    void set_times(nanoseconds_t sw_time, nanoseconds_t hw_time, nanoseconds_t hw_buf);

    // get time when the frame was passed to or received from the OS
    nanoseconds_t sw_frame_time() const;

    // get time when the frame was passed to or received from hardware
    // this method takes into account OS buffer size and probably ADC/DAC
    nanoseconds_t hw_frame_time() const;

    // get time when the sample inside frame was passed to or received from the OS
    nanoseconds_t sw_sample_time(size_t sample_index) const;

    // get time when the sample inside frame was passed to or received from hardware
    // this method takes into account OS buffer size and position of sample in frame
    nanoseconds_t hw_sample_time(size_t sample_index) const;

    // get hardware buffer length at the time when frame was captured or played
    nanoseconds_t hw_buf_len() const;

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

    Dir dir_ {};
    size_t dev_index_ {};

    std::vector<sample_t> data_;
    size_t size_ {};

    nanoseconds_t sw_time_ {};
    nanoseconds_t hw_time_ {};
    nanoseconds_t hw_buf_ {};

    std::atomic<int> refcount_ {};
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
