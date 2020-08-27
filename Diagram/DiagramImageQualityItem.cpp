#include "DiagramImageQualityItem.h"

#include <cmath>
#include <float.h>
#include <QPainter>
#include <QDomDocument>
#include <QGraphicsOpacityEffect>

#include "../Core/GlobalFunc.h"
#include "../Core/View.h"

DiagramImageQualityItem::DiagramImageQualityItem(QGraphicsItem* parent)
    : DiagramLineItem(QLineF(), nullptr, parent)
    , _lineWidth(21)
    , _leftRatio(1 / 3.0f)
    , _rightRatio(2 / 3.0f)
{

}

DiagramImageQualityItem::DiagramImageQualityItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent)
    : DiagramLineItem(line, contextMenu, parent)
    , _lineWidth(21)
    , _leftRatio(1 / 3.0f)
    , _rightRatio(2 / 3.0f)
{

}

DiagramImageQualityItem::~DiagramImageQualityItem()
{
    emit itemDeleted();
}

QDomElement DiagramImageQualityItem::saveToXML(QDomDocument& doc)
{
    QDomElement lineItem = doc.createElement("GraphicsItem");
    lineItem.setAttribute("Type", "DiagramLineItem");

    QDomElement attribute = doc.createElement("Attribute");
    attribute.setAttribute("Name", "ImageQuality");
    attribute.setAttribute("Position", pointFToString(pos()));
    attribute.setAttribute("Point1", pointFToString(line().p1()));
    attribute.setAttribute("Point2", pointFToString(line().p2()));
    attribute.setAttribute("Color", colorToString(pen().color()));
    attribute.setAttribute("EndPointColor", colorToString(_endpointPen.color()));
    attribute.setAttribute("LineWidth", QString::number(_lineWidth));
    attribute.setAttribute("LeftRatio", QString::number(_leftRatio, 'f', 3));
    attribute.setAttribute("RightRatio", QString::number(_rightRatio, 'f', 3));
    attribute.setAttribute("Opacity", QString::number(_effect->opacity(), 'f', 2));

    lineItem.appendChild(attribute);
    return lineItem;
}

void DiagramImageQualityItem::loadFromXML(const QDomElement& e)
{
    DiagramLineItem::loadFromXML(e);

    _lineWidth = e.attribute("LineWidth").toInt();
    _leftRatio = e.attribute("LeftRatio").toFloat();
    _rightRatio = e.attribute("RightRatio").toFloat();

    View* view = qobject_cast<View*>(scene()->parent());
    if (view)
    {
        view->showImageQualityDialog(this, _leftRatio, _rightRatio);
    }
}

void DiagramImageQualityItem::setLeftRatio(float ratio)
{
    _leftRatio = ratio;
    scene()->update();
}

void DiagramImageQualityItem::setRightRatio(float ratio)
{
    _rightRatio = ratio;
    scene()->update();
}

void DiagramImageQualityItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    DiagramLineItem::paint(painter, option, widget);

    drawRect(painter);
    drawResizeHandle(painter);
}

void DiagramImageQualityItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    getGlobalActiveView()->showImageQualityDialog(this, _leftRatio, _rightRatio);

    DiagramLineItem::mouseReleaseEvent(event);
}

void DiagramImageQualityItem::drawRect(QPainter* painter)
{
    if (_lineWidth <= 1)
        return;

    QPointF p1 = line().p1();
    QPointF p2 = line().p2();
    QPointF slope = line().p2() - line().p1();
    QPointF orthoSlope(-slope.y(), slope.x());
    if (orthoSlope.x() != 0.0f || orthoSlope.y() != 0.0f)
    {
        orthoSlope /= sqrt(orthoSlope.x() * orthoSlope.x() + orthoSlope.y() * orthoSlope.y());
    }

    QVector<QPointF> vec;
    vec.push_back(p1 - orthoSlope * _lineWidth / 2.0);
    vec.push_back(p1 + orthoSlope * _lineWidth / 2.0);
    vec.push_back(p2 + orthoSlope * _lineWidth / 2.0);
    vec.push_back(p2 - orthoSlope * _lineWidth / 2.0);

    // Draw red lines
    QPolygonF polygon(vec);
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(QBrush(QColor(128, 128, 255, 128)));
    painter->drawConvexPolygon(polygon);

    painter->setPen(QPen(qRgb(255, 100, 100)));
    QPointF left(p1 + _leftRatio * slope);
    painter->drawLine(left - _lineWidth / 2 * orthoSlope, left + _lineWidth / 2 * orthoSlope);
    QPointF right(p1 + _rightRatio * slope);
    painter->drawLine(right - _lineWidth / 2 * orthoSlope, right + _lineWidth / 2 * orthoSlope);

    painter->setRenderHint(QPainter::Antialiasing, false);
}
