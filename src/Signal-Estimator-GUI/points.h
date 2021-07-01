#ifndef GRAPHING_H
#define GRAPHING_H
#include <optional>
#include <iostream>
#include <cstring>
#include "../../3rdparty/Ring-Buffer-master/ringbuffer.hpp"
#include <QVector>
#include <QPointF>

typedef enum {Input,Output} io ;

class PointsBuffer{
public:
    void appendPoint(QPointF);

    QVector<QPointF> getCurrentPoints();
    void clearBuf();

private:
    jnk0le::Ringbuffer<QPointF,512> ringbuf_;
    QVector<QPointF> lastPlottedPoints;

};

struct QPFcompare {
    bool operator ()(const QPointF& a, const QPointF& b)const{
        return a.x() < b.x();
    }
};

#endif // GRAPHING_H
