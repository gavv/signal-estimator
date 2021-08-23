// Copyright (c) Signal Estimator authors
// Licensed under MIT

#include "PointsBuffer.hpp"

#include <algorithm>

void PointsBuffer::append_point(QPointF pt) {
    ringbuf_.push_back(pt);

    if (ringbuf_.back().x() - ringbuf_.front().x() > ring_size_milliseconds_) {
        ringbuf_.pop_front();
    }
}

QVector<QPointF> PointsBuffer::get_current_points() {
    QVector<QPointF> ret((int)ringbuf_.size());
    std::copy_n(ringbuf_.begin(), ringbuf_.size(), ret.begin());

    return ret;
}

void PointsBuffer::clear_buf() {
    ringbuf_.clear();
}
