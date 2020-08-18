#pragma once

#include <QGraphicsPolygonItem>
#include <QDomElement>

QT_BEGIN_NAMESPACE
class QGraphicsOpacityEffect;
class QDomDocument;
QT_END_NAMESPACE

class DiagramItem : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 15 };
    enum DiagramType
    {
        None, Rect, RoundRect, Circle, Ellipse, Rhombus, Parallelogram, Text, Line, Arrow, Plot, Angle
    };
    enum Direction { TopLeft = 0, Top, TopRight, Left, Right, BottomLeft, Bottom, BottomRight };

    DiagramItem();
    DiagramItem(DiagramType diagramType, QMenu* contextMenu = nullptr, QGraphicsItem* parent = nullptr);
    ~DiagramItem();

    DiagramType diagramType() const { return _diagramType; }
    QPolygonF polygon() const { return _polygon; }

    QPixmap image() const;
    int type() const override { return Type;}
    DiagramItem* clone();

    void setRectF(const QRectF& rect);

    void setTransparency(int value);
    int transparency();

    void setDrawingFinished(bool finished);

    virtual QDomElement saveToXML(QDomDocument& doc);

    virtual void loadFromXML(const QDomElement& e);

    void statisticsInfo();

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
    QPolygonF scaledPolygon(const QPolygonF& old, Direction direction, const QPointF& newPos);

    int changeIndex(int index);

    QList<QPointF> resizeHandlePoints();

    bool isCloseEnough(const QPointF& p1, const QPointF& p2);

private:
    DiagramType _diagramType;
    QPolygonF _polygon;
    QGraphicsOpacityEffect* _effect;
    QMenu* _contextMenu;
    static constexpr qreal resizePointWidth = 5;
    static constexpr qreal closeEnoughDistance = 12;
    bool _resizeMode = false;
    Direction _scaleDirection;
    bool _drawingFinished = false;
    int _previousMode;

    QString _info;
};
