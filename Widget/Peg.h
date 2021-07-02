#pragma once

#include <QPoint>
#include <QRect>

#define PEG_RADIUS      7

class Peg
{
public:
    Peg() {}

    Peg(QPoint point) { _point = QPointF(qreal(point.x()) / _size, qreal(point.y()) / _size); }

    Peg(int x, int y) { _point = QPointF(qreal(x) / _size, qreal(y) / _size); }

    Peg(qreal x, qreal y) { _point = QPointF(x, y); }

public:
    qreal realX() const { return _point.x(); }
    qreal realY() const { return _point.y(); }

    int x() const { return _point.x() * _size; }
    int y() const { return _point.y() * _size; }

    QPoint point() const { return QPoint(_point.x() * _size, _point.y() * _size); }

    // Get point with offset
    QPoint point(const QPoint& point) const
    {
        return QPoint(_point.x() * _size + point.x(), _point.y() * _size + point.y());
    }

    // Get rect
    QRect rect() const
    {
        return QRect(x() - PEG_RADIUS, y() - PEG_RADIUS, PEG_RADIUS * 2, PEG_RADIUS * 2);
    }

    void setPoint(const QPoint& point)  { _point = QPointF(qreal(point.x()) / _size, qreal(point.y()) / _size); }

    static void setSize(int size) { _size = size; }

private:
    QPointF _point;

    static int _size;
};
