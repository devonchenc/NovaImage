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

	DiagramLineItem(int type, const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
	~DiagramLineItem();

	int type() const override { return Type; }

	void setEndpointPen(const QPen& pen);
	QPen pointPen() const;

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
	QList<QPointF> resizeHandlePoints();

	bool isCloseEnough(const QPointF& p1, const QPointF& p2);

	float length() const;

	QString lengthString() const;

	void drawLengthText(QPainter* painter);

	void drawArrow(QPainter* painter);

private:
	int _type;			// 0:Line, 1:Arrow
    QMenu* _contextMenu;
	QPen _endpointPen;
	static constexpr qreal closeEnoughDistance = 12;
	bool _resizeMode = false;
	Index _dragIndex;
	bool _drawingFinished = false;
	int _previousMode;
};
