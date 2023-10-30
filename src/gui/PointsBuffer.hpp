// Copyright (c) Signal Estimator authors
// Licensed under MIT

#pragma once

#include <QPointF>
#include <QVector>

#include <deque>

namespace signal_estimator {

class PointsBuffer {
public:
    void append_point(QPointF);

    QVector<QPointF> get_current_points();
    void clear_buf();

    int size_milliseconds() const {
        return ring_size_milliseconds_;
    }

private:
    const int ring_size_milliseconds_ = 10000;

    std::deque<QPointF> ringbuf_;
};

} // namespace signal_estimator
