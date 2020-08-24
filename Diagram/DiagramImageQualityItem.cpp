#include "DiagramImageQualityItem.h"

#include <float.h>
#include <QPainter>
#include <QDomDocument>
#include <QGraphicsOpacityEffect>

#include "../Core/GlobalFunc.h"
#include "../Core/View.h"

DiagramImageQualityItem::DiagramImageQualityItem()
    : DiagramLineItem(QLineF(), nullptr, nullptr)
    , _lineWidth(21)
{

}

DiagramImageQualityItem::DiagramImageQualityItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent)
    : DiagramLineItem(line, contextMenu, parent)
    , _lineWidth(21)
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
    attribute.setAttribute("Name", "Arrow");
    attribute.setAttribute("Position", pointFToString(pos()));
    attribute.setAttribute("Point1", pointFToString(line().p1()));
    attribute.setAttribute("Point2", pointFToString(line().p2()));
    attribute.setAttribute("Color", colorToString(pen().color()));
    attribute.setAttribute("EndPointColor", colorToString(_endpointPen.color()));
    attribute.setAttribute("LineWidth", QString::number(_lineWidth));
    attribute.setAttribute("Opacity", QString::number(_effect->opacity(), 'f', 2));

    lineItem.appendChild(attribute);
    return lineItem;
}

void DiagramImageQualityItem::loadFromXML(const QDomElement& e)
{
    DiagramLineItem::loadFromXML(e);

    _lineWidth = e.attribute("LineWidth").toInt();
}

void DiagramImageQualityItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    DiagramLineItem::paint(painter, option, widget);

    drawRect(painter);
    drawResizeHandle(painter);
}

void DiagramImageQualityItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    getGlobalActiveView()->showImageQualityDialog(this);

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

    QPolygonF polygon(vec);
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(QBrush(QColor(128, 128, 255, 128)));
    painter->drawConvexPolygon(polygon);

    painter->setRenderHint(QPainter::Antialiasing, false);
}