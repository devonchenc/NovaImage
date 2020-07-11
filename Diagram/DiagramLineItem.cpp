#include "DiagramLineItem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "../Core/GlobalFunc.h"
#include "../Core/View.h"
#include "../Core/GraphicsView.h"
#include "../Core/GraphicsScene.h"
#include "../Image/BaseImage.h"

DiagramLineItem::DiagramLineItem(int type, const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent)
	: QGraphicsLineItem(line, parent)
	, _type(type)
	, _contextMenu(contextMenu)
	, _previousMode(MOVE_ITEM)
{
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setAcceptHoverEvents(true);
}

DiagramLineItem::~DiagramLineItem()
{

}

void DiagramLineItem::setEndpointPen(const QPen& pen)
{
	_endpointPen = pen;
}

QPen DiagramLineItem::pointPen() const
{
	return _endpointPen;
}

void DiagramLineItem::setDrawingFinished(bool finished)
{
	_drawingFinished = finished;
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
	if (scene->mode() != MOVE_ITEM && scene->mode() != MOVE_ITEM_TEMP)
		return;

	_resizeMode = false;
	int index = 0;
	foreach (const QPointF& p, resizeHandlePoints())
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
	if (scene->mode() != MOVE_ITEM && scene->mode() != MOVE_ITEM_TEMP)
		return;

	if (_resizeMode)
	{
		prepareGeometryChange();

		if (_dragIndex == Point1)
		{
			setLine(QLineF(event->pos(), line().p2()));
		}
		else
		{
			setLine(QLineF(line().p1(), event->pos()));
		}
	}

	scene->update();

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
	bool closeToHandlerPoint = false;
	QList<QPointF> pointList = resizeHandlePoints();
	foreach (const QPointF& p, pointList)
	{
		if (isCloseEnough(p, event->pos()))
		{
			setCursor(Qt::SizeAllCursor);
			closeToHandlerPoint = true;
		}
	}

	if (_drawingFinished)
	{
		GraphicsScene* scene = dynamic_cast<GraphicsScene*>(this->scene());
		if (closeToHandlerPoint)
		{
			// Close to handler points
			scene->setMode(MOVE_ITEM_TEMP);
		}
		else
		{
			scene->setMode(_previousMode);
		}
	}

	QGraphicsLineItem::hoverMoveEvent(event);
}

void DiagramLineItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	GraphicsScene* scene = dynamic_cast<GraphicsScene*>(this->scene());
	_previousMode = scene->mode();

	QGraphicsLineItem::hoverEnterEvent(event);
}

void DiagramLineItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	// Restore mode
	GraphicsScene* scene = dynamic_cast<GraphicsScene*>(this->scene());
	scene->setMode(_previousMode);

	QGraphicsLineItem::hoverLeaveEvent(event);
}

void DiagramLineItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setRenderHint(QPainter::Antialiasing, true);

    // remove build-in selected state
    QStyleOptionGraphicsItem myOption(*option);
	myOption.state &= ~QStyle::State_Selected;
	QGraphicsLineItem::paint(painter, &myOption, widget);

	painter->setRenderHint(QPainter::Antialiasing, false);
	if (_type == 0)
	{
		// Draw resize handles
		qreal resizePointWidth = 6;
		painter->setPen(_endpointPen);
		foreach (const QPointF& point, resizeHandlePoints())
		{
			painter->drawLine(point.x(), point.y() - resizePointWidth, point.x(), point.y() + resizePointWidth);
			painter->drawLine(point.x() - resizePointWidth, point.y(), point.x() + resizePointWidth, point.y());
		}

		drawLengthText(painter);
	}
	else if (_type == 1)
	{
		drawArrow(painter);
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

QList<QPointF> DiagramLineItem::resizeHandlePoints()
{
	return QList<QPointF>{line().p1(), line().p2()};
}

bool DiagramLineItem::isCloseEnough(const QPointF& p1, const QPointF& p2)
{
	qreal delta = std::sqrtf((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()));
	return delta < closeEnoughDistance;
}

float DiagramLineItem::length() const
{
	float offsetX = line().p1().x() - line().p2().x();
	float offsetY = line().p1().y() - line().p2().y();
	return sqrt(offsetX * offsetX + offsetY * offsetY);
}

QString DiagramLineItem::lengthString() const
{
	float offsetX = line().p1().x() - line().p2().x();
	float offsetY = line().p1().y() - line().p2().y();

	BaseImage* image = getGlobalImage();
	if (image && image->hasPixelSpacing())
	{
		float horzPixelSpacing = image->horzPixelSpacing();
		float vertPixelSpacing = image->vertPixelSpacing();
		offsetX *= horzPixelSpacing;
		offsetY *= vertPixelSpacing;
	}
	float length = sqrt(offsetX * offsetX + offsetY * offsetY);

	QString str = QString::number(length, 'f', 2);
	if (image && image->hasPixelSpacing())
	{
		str += " mm";
	}
	else
	{
		str += tr(" px");
	}
	return str;
}

void DiagramLineItem::drawLengthText(QPainter* painter)
{
	QTransform transform = getGlobalView()->view()->transform();
	QTransform transform2;
	// The output text is always near the point on the right
	if (line().p1().x() < line().p2().x())
	{
		transform2.translate(line().p2().x() + 10, line().p2().y() + 5);
	}
	else
	{
		transform2.translate(line().p1().x() + 10, line().p1().y() + 5);
	}

	painter->setWorldTransform(transform.inverted() * transform2, true);
	painter->setFont(QFont("Arial", 10));
	painter->setPen(QPen(Qt::yellow));
	painter->drawText(0, 0, lengthString());
}

void DiagramLineItem::drawArrow(QPainter* painter)
{
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->setPen(pen());

	float len = length();

	float cosT = (len > FLT_EPSILON) ? (line().p2().x() - line().p1().x()) / len : 1.0f;
	float sinT = (len > FLT_EPSILON) ? (line().p2().y() - line().p1().y()) / len : 0.0f;

	int x = 18;
	int y = 7;
	QPointF point1(line().p2().x() - x * cosT - y * sinT, line().p2().y() - x * sinT + y * cosT);
	QPointF point2(line().p2().x() - x * cosT + y * sinT, line().p2().y() - x * sinT - y * cosT);
	painter->drawLine(line().p2(), point1);
	painter->drawLine(line().p2(), point2);
}