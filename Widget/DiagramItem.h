#pragma once

#include <QGraphicsPolygonItem>

QT_BEGIN_NAMESPACE
class QGraphicsOpacityEffect;
QT_END_NAMESPACE

class DiagramItem : public QObject, public QGraphicsPolygonItem
{
	Q_OBJECT

public:
    enum { Type = UserType + 15 };
	enum DiagramType
	{
		None, Rect, RoundRect, Circle, Ellipse, Rhombus, Parallelogram, Text, Line, Arrow
	};
	enum Direction { TopLeft = 0, Top, TopRight, Left, Right, BottomLeft, Bottom, BottomRight };

    DiagramItem(DiagramType diagramType, QMenu* contextMenu = nullptr, QGraphicsItem* parent = nullptr);
	~DiagramItem();

    DiagramType diagramType() const { return _diagramType; }
    QPolygonF polygon() const { return _polygon; }

    QPixmap image() const;
    int type() const override { return Type;}
    DiagramItem* clone();

	void setRectF(const QRectF& rect);

	void setDrawingFinished(bool finished);

signals:
	void itemSelectedChange(QGraphicsItem* item);

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
    QPolygonF scaledPolygon(QPolygonF const& old, Direction direction, QPointF const& newPos);

	int changeIndex(int index);

	QList<QPointF> resizeHandlePoints();

	bool isCloseEnough(QPointF const& p1, QPointF const& p2);

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
};

