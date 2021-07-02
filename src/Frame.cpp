/* Copyright (c) 2020 Victor Gaydov
 *
 * This code is licensed under the MIT License.
 */

#include "Frame.hpp"

#include <cassert>

namespace signal_estimator {

Frame::Frame(const Config& config)
    : config_(config)
    , data_(config_.period_size) {
}

sample_t* Frame::data() {
    return &data_[0];
}

size_t Frame::size() const {
    return data_.size();
}

void Frame::mark_io_begin(IOType type) {
    io_type_ = type;
    io_begin_ts_ = monotonic_timestamp_ns();
}

void Frame::mark_io_end() {
    io_end_ts_ = monotonic_timestamp_ns();
}

nanoseconds_t Frame::sw_frame_time() const {
    switch (io_type_) {
    case IOType::Output:
        return io_begin_ts_;

    case IOType::Input:
        return io_end_ts_;
    }

    assert(!"unexpected type");
    return 0;
}

nanoseconds_t Frame::hw_sample_time(size_t offset) const {
    switch (io_type_) {
    case IOType::Output:
        return io_end_ts_
            + nanoseconds_t((config_.n_periods - 1) * config_.period_size + offset)
            / config_.n_channels * 1000000000 / config_.sample_rate;

    case IOType::Input:
        return io_end_ts_
            - nanoseconds_t(config_.period_size - offset) / config_.n_channels
            * 1000000000 / config_.sample_rate;
    }

    assert(!"unexpected type");
    return 0;
}

} // namespace signal_estimator
