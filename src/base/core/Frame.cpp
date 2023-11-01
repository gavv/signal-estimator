// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/Frame.hpp"
#include "core/FramePool.hpp"

#include <cassert>

namespace signal_estimator {

Frame::Frame(const Config& config, FramePool& pool)
    : config_(config)
    , pool_(pool) {
    data_.reserve(config_.frame_size);
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

void Frame::reset(Dir dir, size_t dev_index) {
    dir_ = dir;
    dev_index_ = dev_index;

    size_ = dev_info().period_size;
    assert(size_ > 0);
    if (data_.size() < size_) {
        data_.resize(size_);
    }
    std::fill_n(data_.data(), size_, 0);

    set_times(0, 0, 0, 0, 0);
}

size_t Frame::dev_index() const {
    return dev_index_;
}

const DevInfo& Frame::dev_info() const {
    switch (dir_) {
    case Dir::Output:
        assert(dev_index_ == 0);
        return config_.output_info;

    case Dir::Input:
        break;
    }

    assert(dev_index_ < config_.input_info.size());
    return config_.input_info[dev_index_];
}

Dir Frame::dir() const {
    return dir_;
}

size_t Frame::size() const {
    return size_;
}

const sample_t* Frame::data() const {
    assert(size_ > 0);
    return &data_[0];
}

sample_t* Frame::data() {
    assert(size_ > 0);
    return &data_[0];
}

void Frame::set_times(nanoseconds_t sw_time, nanoseconds_t hw_time, nanoseconds_t wc_time,
    nanoseconds_t sw_delay, nanoseconds_t hw_delay) {
    sw_time_ = sw_time;
    hw_time_ = hw_time;
    wc_time_ = wc_time;
    sw_delay_ = sw_delay;
    hw_delay_ = hw_delay;
}

nanoseconds_t Frame::sw_frame_time() const {
    return sw_time_;
}

nanoseconds_t Frame::hw_frame_time() const {
    return hw_time_;
}

nanoseconds_t Frame::wc_frame_time() const {
    return wc_time_;
}

nanoseconds_t Frame::sw_sample_time(size_t sample_index) const {
    return sw_time_ + config_.samples_to_ns(sample_index);
}

nanoseconds_t Frame::hw_sample_time(size_t sample_index) const {
    return hw_time_ + config_.samples_to_ns(sample_index);
}

nanoseconds_t Frame::wc_sample_time(size_t sample_index) const {
    return wc_time_ + config_.samples_to_ns(sample_index);
}

nanoseconds_t Frame::sw_delay() const {
    return sw_delay_;
}

nanoseconds_t Frame::hw_delay() const {
    return hw_delay_;
}

} // namespace signal_estimator
