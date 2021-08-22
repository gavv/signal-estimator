#include "PointsBuffer.hpp"



void PointsBuffer::appendPoint(QPointF pt)
{
    if (ringbuf_.size() >= ring_size_)
        ringbuf_.pop_back(); // remove from tail
    ringbuf_.push_front(pt); // insert at head
}

QVector<QPointF> PointsBuffer::getCurrentPoints()
{
    lastPlottedPoints = ringbuf_.toVector();
    return lastPlottedPoints;
}

void PointsBuffer::clearBuf()
{
    // clear both
    ringbuf_.clear();
    lastPlottedPoints.clear();
}
