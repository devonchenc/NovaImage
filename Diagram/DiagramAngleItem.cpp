#include "DiagramAngleItem.h"

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
#include "../Image/BaseImage.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

DiagramAngleItem::DiagramAngleItem(QGraphicsItem* parent)
    : QGraphicsLineItem(QLineF(), parent)
    , _angle(0)
    , _contextMenu(nullptr)
    , _drawingIndex(Point1)
    , _previousMode(MOVE_ITEM)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);

    _effect = new QGraphicsOpacityEffect(this);
    _effect->setOpacity(1.0);
    setGraphicsEffect(_effect);
}

DiagramAngleItem::DiagramAngleItem(const QPointF& startPoint, QMenu* contextMenu, QGraphicsItem* parent)
    : QGraphicsLineItem(QLineF(startPoint, startPoint), parent)
    , _p3(startPoint)
    , _angle(0)
    , _contextMenu(contextMenu)
    , _drawingIndex(Point1)
    , _previousMode(MOVE_ITEM)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);

    _effect = new QGraphicsOpacityEffect(this);
    _effect->setOpacity(1.0);
    setGraphicsEffect(_effect);
}

void DiagramAngleItem::setEndpointPen(const QPen& pen)
{
    _endpointPen = pen;
}

QPen DiagramAngleItem::pointPen() const
{
    return _endpointPen;
}

void DiagramAngleItem::setTransparency(int value)
{
    _effect->setOpacity(1.0 - float(value) / 100);
}

int DiagramAngleItem::transparency() const
{
    return 100 - round(_effect->opacity() * 100);
}

void DiagramAngleItem::setCurrentDrawingPoint(const QPointF& point)
{
    if (_drawingIndex == Point2)
    {
        setLine(QLineF(line().p1(), point));
        _p3 = point;
    }
    else if (_drawingIndex == Point3)
    {
        _p3 = point;
    }
}

void DiagramAngleItem::setDrawingFinished(bool finished)
{
    _drawingFinished = finished;
}

QDomElement DiagramAngleItem::saveToXML(QDomDocument& doc) const
{
    QDomElement lineItem = doc.createElement("GraphicsItem");
    lineItem.setAttribute("Type", "DiagramAngleItem");

    QDomElement attribute = doc.createElement("Attribute");
    attribute.setAttribute("Position", pointFToString(pos()));
    attribute.setAttribute("Point1", pointFToString(p1()));
    attribute.setAttribute("Point2", pointFToString(p2()));
    attribute.setAttribute("Point3", pointFToString(p3()));
    attribute.setAttribute("Color", colorToString(pen().color()));
    attribute.setAttribute("EndPointColor", colorToString(_endpointPen.color()));
    attribute.setAttribute("Opacity", QString::number(_effect->opacity(), 'f', 2));

    lineItem.appendChild(attribute);
    return lineItem;
}

void DiagramAngleItem::loadFromXML(const QDomElement& e)
{
    setPos(stringToPointF(e.attribute("Position")));

    QPointF p1 = stringToPointF(e.attribute("Point1"));
    QPointF p2 = stringToPointF(e.attribute("Point2"));
    _p3 = stringToPointF(e.attribute("Point3"));
    setLine(QLineF(p1, p2));

    QColor color = stringToColor(e.attribute("Color"));
    setPen(QPen(color, 2));

    color = stringToColor(e.attribute("EndPointColor"));
    _endpointPen = QPen(color);

    _effect->setOpacity(e.attribute("Opacity").toDouble());

    _drawingFinished = true;
}

inline QPointF DiagramAngleItem::p1() const
{
    return line().p1();
}

inline QPointF DiagramAngleItem::p2() const
{
    return line().p2();
}

inline QPointF DiagramAngleItem::p3() const
{
    return _p3;
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

void DiagramAngleItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    GraphicsScene* scene = qobject_cast<GraphicsScene*>(this->scene());
    if (scene->mode() != MOVE_ITEM && scene->mode() != MOVE_ITEM_TEMP)
        return;

    bool resizeMode = false;
    int index = 0;
    foreach (const QPointF& p, resizeHandlePoints())
    {
        if (isCloseEnough(event->pos(), p))
        {
			resizeMode = true;
            break;
        }
        index++;
    }

    setFlag(GraphicsItemFlag::ItemIsMovable, !resizeMode);

    _dragIndex = static_cast<Index>(index);

    QGraphicsLineItem::mousePressEvent(event);
}

void DiagramAngleItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    GraphicsScene* scene = qobject_cast<GraphicsScene*>(this->scene());
    if (scene->mode() != MOVE_ITEM && scene->mode() != MOVE_ITEM_TEMP)
        return;

    if (_dragIndex == Point1 || _dragIndex == Point2 || _dragIndex == Point3)
    {
        prepareGeometryChange();

        if (_dragIndex == Point1)
        {
            setLine(QLineF(event->pos(), line().p2()));
        }
        else if (_dragIndex == Point2)
        {
            setLine(QLineF(line().p1(), event->pos()));
        }
        else if (_dragIndex == Point3)
        {
            _p3 = event->pos();
        }
    }

    scene->update();

    QGraphicsLineItem::mouseMoveEvent(event);
}

void DiagramAngleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    _dragIndex = DragNone;

    emit itemChanged();

    QGraphicsLineItem::mouseReleaseEvent(event);
}

void DiagramAngleItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
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
        GraphicsScene* scene = qobject_cast<GraphicsScene*>(this->scene());
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

void DiagramAngleItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    GraphicsScene* scene = qobject_cast<GraphicsScene*>(this->scene());
    _previousMode = scene->mode();

    QGraphicsLineItem::hoverEnterEvent(event);
}

void DiagramAngleItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    // Restore mode
    GraphicsScene* scene = qobject_cast<GraphicsScene*>(this->scene());
    scene->setMode(_previousMode);

    QGraphicsLineItem::hoverLeaveEvent(event);
}

void DiagramAngleItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    QGraphicsLineItem::paint(painter, option, widget);

    painter->setPen(this->pen());
    painter->drawLine(p2(), p3());

    painter->setRenderHint(QPainter::Antialiasing, false);

    // Draw resize handles
    qreal resizePointWidth = 6;
    painter->setPen(_endpointPen);
    foreach (const QPointF& point, resizeHandlePoints())
    {
        painter->drawLine(point.x(), point.y() - resizePointWidth, point.x(), point.y() + resizePointWidth);
        painter->drawLine(point.x() - resizePointWidth, point.y(), point.x() + resizePointWidth, point.y());
    }

    if (_drawingFinished || _drawingIndex == Point3)
    {
        calcAngle();
        drawAngleText(painter);
    }
}

void DiagramAngleItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    scene()->clearSelection();
    setSelected(true);
    _contextMenu->exec(event->screenPos());
}

QVariant DiagramAngleItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        emit itemSelectedChange(this);

    return value;
}

QRectF DiagramAngleItem::boundingRect() const
{
    qreal left = qMin(qMin(p1().x(), p2().x()), p3().x());
    qreal right = qMax(qMax(p1().x(), p2().x()), p3().x());
    qreal top = qMin(qMin(p1().y(), p2().y()), p3().y());
    qreal bottom = qMax(qMax(p1().y(), p2().y()), p3().y());
    return QRectF(left, top, right - left, bottom - top);
}

QPainterPath DiagramAngleItem::shape() const
{
    QPainterPath path;
    if (line() == QLineF())
        return path;

    // Add two lines
    path.moveTo(p1());
    path.lineTo(p2());
    path.moveTo(p2());
    path.lineTo(p3());

    return qt_graphicsItem_shapeFromPath(path, pen());
}

QList<QPointF> DiagramAngleItem::resizeHandlePoints() const
{
    return QList<QPointF>{p1(), p2(), p3()};
}

bool DiagramAngleItem::isCloseEnough(const QPointF& p1, const QPointF& p2) const
{
    qreal delta = sqrtf((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()));
    return delta < closeEnoughDistance;
}

void DiagramAngleItem::calcAngle()
{
    float a1 = float(p3().x() - p2().x());
    float b1 = float(p3().y() - p2().y());
    float a2 = float(p1().x() - p2().x());
    float b2 = float(p1().y() - p2().y());
    if ((a1 == 0 && b1 == 0) || (a2 == 0 && b2 == 0))
    {
        _angle = 0;
    }
    else
    {
        float fTemp = sqrt(a1 * a1 + b1 * b1) * sqrt(a2 * a2 + b2 * b2);
        _angle = acos((a1 * a2 + b1 * b2) / fTemp) * 180.0f / M_PI;
    }
}

void DiagramAngleItem::drawAngleText(QPainter* painter)
{
    QGraphicsScene* graphicsScene = scene();
    QList<QGraphicsView*> viewList = graphicsScene->views();
    Q_ASSERT(viewList.size() > 0);

    QTransform transform = viewList.first()->transform();
    QTransform transform2;
    if ((p3().x() > p2().x() && p3().x() - p2().x() > 6 * abs((p3().y() - p2().y()))) ||
            (p1().x() > p2().x() && p1().x() - p2().x() > 6 * abs((p1().y() - p2().y()))))
    {
        transform2.translate(p2().x(), p2().y() - 10);
    }
    else
    {
        transform2.translate(p2().x() + 10, p2().y() + 5);
    }
    painter->setWorldTransform(transform.inverted() * transform2, true);
    painter->setFont(QFont("Arial", 10));
    painter->setPen(QPen(Qt::yellow));
    QString angle = QString::number(_angle, 'f', 2);
    painter->drawText(0, 0, angle + QString(QChar(0x00B0)));
}

// Copied from qgraphicsitem.cpp source code
QPainterPath DiagramAngleItem::qt_graphicsItem_shapeFromPath(const QPainterPath& path, const QPen& pen) const
{
    // We unfortunately need this hack as QPainterPathStroker will set a width of 1.0
    // if we pass a value of 0.0 to QPainterPathStroker::setWidth()
    const qreal penWidthZero = qreal(0.00000001);
    if (path == QPainterPath() || pen == Qt::NoPen)
        return path;
    QPainterPathStroker ps;
    ps.setCapStyle(pen.capStyle());
    if (pen.widthF() <= 0.0)
        ps.setWidth(penWidthZero);
    else
        ps.setWidth(pen.widthF());
    ps.setJoinStyle(pen.joinStyle());
    ps.setMiterLimit(pen.miterLimit());
    QPainterPath p = ps.createStroke(path);
    p.addPath(path);
    return p;
}
