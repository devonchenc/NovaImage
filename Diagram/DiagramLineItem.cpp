#include "DiagramLineItem.h"

#include <cmath>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsOpacityEffect>

#include "../Core/GlobalFunc.h"
#include "../Core/View.h"
#include "../Core/GraphicsView.h"
#include "../Core/GraphicsScene.h"

DiagramLineItem::DiagramLineItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent)
    : QGraphicsLineItem(line, parent)
    , _contextMenu(contextMenu)
    , _previousMode(MOVE_ITEM)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);

    _effect = new QGraphicsOpacityEffect;
    _effect->setOpacity(1.0);
    setGraphicsEffect(_effect);
}

DiagramLineItem::~DiagramLineItem()
{
    delete _effect;
}

void DiagramLineItem::setEndpointPen(const QPen& pen)
{
    _endpointPen = pen;
}

QPen DiagramLineItem::pointPen() const
{
    return _endpointPen;
}

void DiagramLineItem::setTransparency(int value)
{
    _effect->setOpacity(1.0 - float(value) / 100);
}

int DiagramLineItem::transparency()
{
    return 100 - round(_effect->opacity() * 100);
}

void DiagramLineItem::setDrawingFinished(bool finished)
{
    _drawingFinished = finished;
}

void DiagramLineItem::loadFromXML(const QDomElement& e)
{
    setPos(stringToPointF(e.attribute("Position")));

    QPointF p1 = stringToPointF(e.attribute("Point1"));
    QPointF p2 = stringToPointF(e.attribute("Point2"));
    setLine(QLineF(p1, p2));

    QColor color = stringToColor(e.attribute("Color"));
    setPen(QPen(color, 2));

    color = stringToColor(e.attribute("EndPointColor"));
    _endpointPen = QPen(color);

    _effect->setOpacity(e.attribute("Opacity").toDouble());

    _drawingFinished = true;
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

    emit itemChanged();

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

    QGraphicsLineItem::paint(painter, option, widget);

    painter->setRenderHint(QPainter::Antialiasing, false);
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
    qreal delta = sqrtf((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()));
    return delta < closeEnoughDistance;
}

// Draw resize handles
void DiagramLineItem::drawResizeHandle(QPainter* painter)
{
    qreal resizePointWidth = 6;
    painter->setPen(_endpointPen);
    foreach(const QPointF& point, resizeHandlePoints())
    {
        painter->drawLine(point.x(), point.y() - resizePointWidth, point.x(), point.y() + resizePointWidth);
        painter->drawLine(point.x() - resizePointWidth, point.y(), point.x() + resizePointWidth, point.y());
    }
}

float DiagramLineItem::length() const
{
    float offsetX = line().p1().x() - line().p2().x();
    float offsetY = line().p1().y() - line().p2().y();
    return sqrt(offsetX * offsetX + offsetY * offsetY);
}
