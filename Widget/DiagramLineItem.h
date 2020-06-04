#pragma once

#include <QGraphicsLineItem>
#include <QPen>

class DiagramLineItem : public QObject, public QGraphicsLineItem
{
	Q_OBJECT

public:
    enum { Type = UserType + 12 };
	enum Index
	{
		Point1, Point2
	};

	DiagramLineItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
	~DiagramLineItem();

	int type() const override { return Type; }

	void setPointPen(const QPen& pen);
	QPen pointPen() const;

signals:
	void itemSelectedChange(QGraphicsItem* item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
	QList<QPointF> resizeHandlePoints();

	bool isCloseEnough(QPointF const& p1, QPointF const& p2);

private:
    QMenu* _contextMenu;
	QPen _pointPen;
	static constexpr qreal resizeHandlePointWidth = 7;
	static constexpr qreal closeEnoughDistance = 20;
	bool _resizeMode = false;
	Index _dragIndex;
};
