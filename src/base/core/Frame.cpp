// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/Frame.hpp"
#include "core/FramePool.hpp"

#include <cassert>

namespace signal_estimator {

Frame::Frame(const Config& config, FramePool& pool)
    : config_(config)
    , pool_(pool)
    , data_(config_.io_period_size) {
    clear();
}

int Frame::get_ref() const {
    return refcount_;
}

void Frame::add_ref() {
    const int oldcount = refcount_++;

    (void)oldcount;
    assert(oldcount >= 0);
}

void Frame::sub_ref() {
    const int oldcount = refcount_--;

    assert(oldcount > 0);

    if (oldcount == 1) {
        pool_.release_frame_(this);
    }
}

void Frame::clear() {
    std::fill(data_.begin(), data_.end(), sample_t(0));

    io_type_ = FrameType::Output;
    io_time_ = 0;
}

size_t Frame::size() const {
    return data_.size();
}

const sample_t* Frame::data() const {
    assert(data_.size() > 0);
    return &data_[0];
}

sample_t* Frame::data() {
    assert(data_.size() > 0);
    return &data_[0];
}

FrameType Frame::type() const {
    return io_type_;
}

void Frame::set_type(FrameType type) {
    io_type_ = type;
}

void Frame::set_time() {
    io_time_ = monotonic_timestamp_ns();
}

nanoseconds_t Frame::sw_frame_time() const {
    return io_time_;
}

nanoseconds_t Frame::hw_frame_time() const {
    switch (io_type_) {
    case FrameType::Output:
        return io_time_
            + config_.samples_to_ns(
                (config_.io_num_periods - 1) * config_.io_period_size);

    case FrameType::Input:
        return io_time_ - config_.samples_to_ns(config_.io_period_size);
    }

    return 0;
}

nanoseconds_t Frame::hw_sample_time(size_t sample_index) const {
    switch (io_type_) {
    case FrameType::Output:
        return io_time_
            + config_.samples_to_ns(
                (config_.io_num_periods - 1) * config_.io_period_size + sample_index);

    case FrameType::Input:
        return io_time_ - config_.samples_to_ns(config_.io_period_size - sample_index);
    }

    return 0;
}

} // namespace signal_estimator
