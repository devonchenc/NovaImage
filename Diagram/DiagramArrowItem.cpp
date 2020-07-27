#include "DiagramArrowItem.h"

#include <QPainter>
#include <QDomDocument>
#include <float.h>

DiagramArrowItem::DiagramArrowItem()
    : DiagramLineItem(QLineF(), nullptr, nullptr)
{

}

DiagramArrowItem::DiagramArrowItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent)
    : DiagramLineItem(line, contextMenu, parent)
{

}

DiagramArrowItem::~DiagramArrowItem()
{

}

QDomElement DiagramArrowItem::saveToXML(QDomDocument* doc)
{
    QDomElement lineItem = doc->createElement("DiagramItem");
    lineItem.setAttribute("Type", "DiagramLineItem");

    QDomElement attribute = doc->createElement("Attribute");
    attribute.setAttribute("Name", "Arrow");
    attribute.setAttribute("Point1", QString("%1 %2").arg(line().p1().x()).arg(line().p1().y()));
    attribute.setAttribute("Point2", QString("%1 %2").arg(line().p2().x()).arg(line().p2().y()));
    attribute.setAttribute("Color", QString("%1 %2 %3").arg(pen().color().red()).arg(pen().color().green()).arg(pen().color().blue()));

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
