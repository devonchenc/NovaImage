#include "DiagramArrowItem.h"

#include <float.h>
#include <QPainter>
#include <QDomDocument>
#include <QGraphicsOpacityEffect>

#include "../Core/GlobalFunc.h"

DiagramArrowItem::DiagramArrowItem(QGraphicsItem* parent)
    : DiagramLineItem(QLineF(), nullptr, parent)
{

}

DiagramArrowItem::DiagramArrowItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent)
    : DiagramLineItem(line, contextMenu, parent)
{

}

QDomElement DiagramArrowItem::saveToXML(QDomDocument& doc) const
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
    attribute.setAttribute("Opacity", QString::number(_effect->opacity(), 'f', 2));

    lineItem.appendChild(attribute);
    return lineItem;
}

void DiagramArrowItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    DiagramLineItem::paint(painter, option, widget);

    drawArrow(painter);
}

void DiagramArrowItem::drawArrow(QPainter* painter)
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
