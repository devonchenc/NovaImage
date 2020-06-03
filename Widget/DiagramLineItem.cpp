#include "DiagramLineItem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

#include "../GraphicsScene.h"

DiagramLineItem::DiagramLineItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent)
	: QGraphicsLineItem(line, parent)
{
	_contextMenu = contextMenu;
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
}

DiagramLineItem::~DiagramLineItem()
{

}

void DiagramLineItem::setPointPen(const QPen& pen)
{
	_pointPen = pen;
}

QPen DiagramLineItem::pointPen() const
{
	return _pointPen;
}

bool DiagramLineItem::isCloseEnough(QPointF const& p1, QPointF const& p2)
{
	qreal delta = std::abs(p1.x() - p2.x()) + std::abs(p1.y() - p2.y());
	return delta < closeEnoughDistance;
}

QList<QPointF> DiagramLineItem::resizeHandlePoints()
{
	return QList<QPointF>{line().p1(), line().p2()};
}

/*
DiagramLineItem* DiagramLineItem::clone()
{
    DiagramItem* cloned = new DiagramItem(_diagramType, pen().color(), brush().color(), _contextMenu, nullptr);
    cloned->_polygon = _polygon;
    cloned->setPos(scenePos());
    cloned->setPolygon(_polygon);
    cloned->setZValue(zValue());
    return cloned;
}*/

void DiagramLineItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	GraphicsScene* scene = dynamic_cast<GraphicsScene*>(this->scene());
	if (scene->mode() != MOVE_ITEM)
		return;

	_resizeMode = false;
	int index = 0;
	foreach (QPointF const& p, resizeHandlePoints())
	{
		if (isCloseEnough(event->pos(), p))
		{
			_resizeMode = true;
			break;
		}
		index++;
	}

	setFlag(GraphicsItemFlag::ItemIsMovable, !_resizeMode);

	_dragIndex = static_cast<Index>(index);

	QGraphicsLineItem::mousePressEvent(event);
}

void DiagramLineItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	GraphicsScene* scene = dynamic_cast<GraphicsScene*>(this->scene());
	if (scene->mode() != MOVE_ITEM)
		return;

	if (_resizeMode)
	{
		prepareGeometryChange();

		if (_dragIndex == Point1)
		{
			QLineF newLine(event->scenePos(), line().p2());
			setLine(newLine);
		}
		else
		{
			QLineF newLine(line().p1(), event->scenePos());
			setLine(newLine);
		}
	}
	QGraphicsLineItem::mouseMoveEvent(event);
}

void DiagramLineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	_resizeMode = false;

	QGraphicsLineItem::mouseReleaseEvent(event);
}

void DiagramLineItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
	setCursor(Qt::ArrowCursor);
	QList<QPointF> pointList = resizeHandlePoints();
	foreach (QPointF const& p, pointList)
	{
		if (isCloseEnough(p, event->pos()))
		{
			setCursor(Qt::SizeAllCursor);
		}
	}
	QGraphicsLineItem::hoverMoveEvent(event);
}

void DiagramLineItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // remove build-in selected state
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;
	QGraphicsLineItem::paint(painter, &myOption, widget);

    // add resize handles
	qreal width = resizeHandlePointWidth;
	foreach (QPointF const& point, resizeHandlePoints())
	{
		painter->setPen(_pointPen);
		painter->drawEllipse(QRectF(point.x() - width / 2, point.y() - width / 2, width, width));
	}
}

void DiagramLineItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    scene()->clearSelection();
    setSelected(true);
    _contextMenu->exec(event->screenPos());
}

QVariant DiagramLineItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (change == QGraphicsItem::ItemSelectedHasChanged)
		emit itemSelectedChange(this);

    return value;
}