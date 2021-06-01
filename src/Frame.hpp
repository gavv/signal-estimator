/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#pragma once

#include "Config.hpp"
#include "Time.hpp"

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

class Frame {
public:
    Frame(const Config&);

    sample_t* data();
    size_t size() const;

    void mark_io_begin(IOType);
    void mark_io_end();

    // get time point when the frame is passed to or received from software ring buffer
    nanoseconds_t sw_frame_time() const;

    // get time point when the sample inside frame is actually played or recorded
    nanoseconds_t hw_sample_time(size_t offset) const;

private:
    const Config& config_;

    IOType io_type_ { IOType::Output };

    nanoseconds_t io_begin_ts_ {};
    nanoseconds_t io_end_ts_ {};

    std::vector<sample_t> data_;
};

} // namespace signal_estimator
