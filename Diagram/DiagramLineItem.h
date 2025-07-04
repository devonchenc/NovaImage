#pragma once

#include <QGraphicsLineItem>
#include <QPen>
#include <QDomElement>

QT_BEGIN_NAMESPACE
class QGraphicsOpacityEffect;
class QDomDocument;
QT_END_NAMESPACE

class DiagramLineItem : public QObject, public QGraphicsLineItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 12 };
    enum Index
    {
        DragNone = -1, Point1, Point2, All
    };

    DiagramLineItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);

    int type() const override { return Type; }

    void setEndpointPen(const QPen& pen);
    QPen pointPen() const;

    void setTransparency(int value);
    int transparency() const;

    void setDrawingFinished(bool finished);

    virtual QDomElement saveToXML(QDomDocument& doc) const = 0;

    virtual void loadFromXML(const QDomElement& e);

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

protected:
    void drawResizeHandle(QPainter* painter);

    float length() const;

private:
    QList<QPointF> resizeHandlePoints() const;

    bool isCloseEnough(const QPointF& p1, const QPointF& p2) const;

protected:
    QPen _endpointPen;
    QGraphicsOpacityEffect* _effect;
    QMenu* _contextMenu;
    static constexpr qreal closeEnoughDistance = 12;
    Index _dragIndex = DragNone;
    bool _drawingFinished = false;
    int _previousMode;
};
