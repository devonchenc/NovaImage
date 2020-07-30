#pragma once

#include <QGraphicsLineItem>
#include <QPen>
#include <QDomElement>

QT_BEGIN_NAMESPACE
class QGraphicsOpacityEffect;
class QDomDocument;
QT_END_NAMESPACE

class DiagramAngleItem : public QObject, public QGraphicsLineItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 10 };
    enum Index
    {
        Point1, Point2, Point3
    };

    DiagramAngleItem();
    DiagramAngleItem(const QPointF& startPoint, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
    ~DiagramAngleItem();

    int type() const override { return Type; }

    QRectF boundingRect() const override;

    QPainterPath shape() const override;

    void setEndpointPen(const QPen& pen);
    QPen pointPen() const;

    void setTransparency(int value);
    int transparency();

    void setCurrentDrawingIndex(Index index) { _drawingIndex = index; }

    void setCurrentDrawingPoint(const QPointF& point);

    Index currentDrawingPoint() { return _drawingIndex; }

    void setDrawingFinished(bool finished);

    virtual QDomElement saveToXML(QDomDocument* doc);

    virtual void loadFromXML(const QDomElement& e);

    inline QPointF p1() const;
    inline QPointF p2() const;
    inline QPointF p3() const;

signals:
    void itemSelectedChange(QGraphicsItem* item);
    void itemChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QList<QPointF> resizeHandlePoints();

    bool isCloseEnough(const QPointF& p1, const QPointF& p2);

    void calcAngle();

    void drawAngleText(QPainter* painter);

    QPainterPath qt_graphicsItem_shapeFromPath(const QPainterPath &path, const QPen &pen) const;

private:
    QPointF _p3;
    float _angle;
    QPen _endpointPen;

    QGraphicsOpacityEffect* _effect;
    QMenu* _contextMenu;
    static constexpr qreal closeEnoughDistance = 12;
    bool _resizeMode = false;
    Index _drawingIndex;
    Index _dragIndex;
    bool _drawingFinished = false;
    int _previousMode;
};
