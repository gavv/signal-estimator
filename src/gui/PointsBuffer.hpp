#pragma once

#include <QPointF>
#include <QVector>

#include <cstring>
#include <iostream>
#include <optional>

class PointsBuffer {
public:
    void append_point(QPointF);

    QVector<QPointF> get_current_points();
    void clear_buf();

private:
    QList<QPointF> ringbuf_; // see appendPoint() implementation
    const size_t ring_size_ = 1024;
    QVector<QPointF> last_plotted_points_;
};
