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

size_t Frame::size() {
    return data_.size();
}

void Frame::set_type(FrameType type) {
    type_ = type;
}

void Frame::set_timestamp() {
    timestamp_ = monotonic_timestamp_ns();
}

nanoseconds_t Frame::sample_time(size_t offset) const {
    switch (type_) {
    case FrameType::Output:
        return timestamp_
            + nanoseconds_t((config_.n_periods - 1) * config_.period_size + offset)
            / config_.n_channels * 1000000000 / config_.sample_rate;

    case FrameType::Input:
        return timestamp_
            - nanoseconds_t(config_.period_size - offset) / config_.n_channels
            * 1000000000 / config_.sample_rate;
    }

    assert(!"unexpected type");
}

} // namespace signal_estimator
