// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include "core/Config.hpp"
#include "core/List.hpp"
#include "core/Pool.hpp"
#include "core/Time.hpp"

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace signal_estimator {

using sample_t = int16_t;

constexpr sample_t MaxSample = +32767;
constexpr sample_t MinSample = -32767;

enum class IOType {
    Output,
    Input,
};

class Frame : public ListNode {
public:
    explicit Frame(const Config&, Pool<Frame>* pool = nullptr);
    virtual ~Frame() = default;
    Frame(Frame&& f) noexcept = default;
    Frame& operator=(Frame&&) noexcept = default;

    sample_t* data();
    size_t size() const;

    void mark_io_begin(IOType);
    void mark_io_end();

    // get time point when the frame is passed to or received from software ring buffer
    nanoseconds_t sw_frame_time() const;

    // get time point when the sample inside frame is actually played or recorded
    nanoseconds_t hw_sample_time(size_t offset) const;

    IOType io_type() {
        return io_type_;
    }

    Pool<Frame>* pool();

    auto begin() {
        return data_.begin();
    }
    auto end() {
        return data_.end();
    }

private:
    const Config* config_;
    Pool<Frame>* pool_;

private:
    IOType io_type_ { IOType::Output };

    nanoseconds_t io_begin_ts_ {};
    nanoseconds_t io_end_ts_ {};

    std::vector<sample_t> data_;
};

} // namespace signal_estimator
