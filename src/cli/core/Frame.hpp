// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/Sample.hpp"
#include "core/Time.hpp"

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
    explicit Frame(const Config&, FramePool&);
    virtual ~Frame() = default;

    Frame(Frame&&) noexcept = default;
    Frame& operator=(Frame&&) noexcept = default;

    // reset frame to initial state
    void clear();

    // frame data and size
    size_t size() const;
    const sample_t* data() const;
    sample_t* data();
    sample_t at(size_t sample_index) const;

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

    // convenience wrappers
    auto begin() {
        return data_.begin();
    }
    auto end() {
        return data_.end();
    }

    // convenience wrappers
    auto operator[](const size_t index) const {
        return data_[index];
    }
    auto& operator[](const size_t index) {
        return data_[index];
    }

private:
    friend class FramePool;

    const Config* config_;
    FramePool* pool_;

    FrameType io_type_;
    nanoseconds_t io_time_;

    std::vector<sample_t> data_;
};

} // namespace signal_estimator
