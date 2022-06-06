// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "core/Frame.hpp"

#include <cassert>

namespace signal_estimator {

Frame::Frame(const Config& config, Pool<Frame>* pool)
    : config_(&config)
    , pool_(pool)
    , data_(config_->io_period_size) {
    assert(data_.size() > 0);
    std::fill(data_.begin(), data_.end(), sample_t(0));
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
            + nanoseconds_t(
                  (config_->io_num_periods - 1) * config_->io_period_size + offset)
            / config_->n_channels * 1000000000 / config_->sample_rate;

    case IOType::Input:
        return io_end_ts_
            - nanoseconds_t((config_->io_num_periods - 1) * config_->io_period_size
                  + (config_->io_period_size - offset))
            / config_->n_channels * 1000000000 / config_->sample_rate;
    }

    assert(!"unexpected type");
    return 0;
}

Pool<Frame>* Frame::pool() {
    return pool_;
}

} // namespace signal_estimator
