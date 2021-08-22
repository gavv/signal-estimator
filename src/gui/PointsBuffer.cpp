#include "PointsBuffer.hpp"

void PointsBuffer::append_point(QPointF pt) {
    if (ringbuf_.size() >= ring_size_)
        ringbuf_.pop_back(); // remove from tail
    ringbuf_.push_front(pt); // insert at head
}

QVector<QPointF> PointsBuffer::get_current_points() {
    last_plotted_points_ = ringbuf_.toVector();
    return last_plotted_points_;
}

void PointsBuffer::clear_buf() {
    // clear both
    ringbuf_.clear();
    last_plotted_points_.clear();
}
