// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Sample.hpp"
#include "core/Time.hpp"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <vector>

namespace signal_estimator {

enum class FrameType {
    Output,
    Input,
};

class FramePool;

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

    const Config& config_;
    FramePool& pool_;

    FrameType io_type_;
    nanoseconds_t io_time_;

    std::vector<sample_t> data_;
};

} // namespace signal_estimator
