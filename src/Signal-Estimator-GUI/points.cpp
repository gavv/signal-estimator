#include "points.h"



void PointsBuffer::appendPoint(QPointF pt)
{
    if (ringbuf_.isFull())
        ringbuf_.remove(); // remove from tail
    ringbuf_.insert(pt); // insert at head
}

QVector<QPointF> PointsBuffer::getCurrentPoints()
{
    if (ringbuf_.readAvailable() <= 0){
        lastPlottedPoints.clear();
        return lastPlottedPoints;
    }
    while (lastPlottedPoints.count() >= ringbuf_.readAvailable()) {
        lastPlottedPoints.pop_front();
    }
    for (int index = 0; index < ringbuf_.readAvailable(); index++){
        if (!lastPlottedPoints.contains(ringbuf_[index])){
            // last plotted points does not have element from ringbuf, append
            lastPlottedPoints.append(ringbuf_[index]);
        }
    }
    return lastPlottedPoints;
}

void PointsBuffer::clearBuf()
{
    // clear both
    ringbuf_.consumerClear();
    lastPlottedPoints.clear();
}
