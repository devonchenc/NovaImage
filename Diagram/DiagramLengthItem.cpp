#include "DiagramLengthItem.h"

#include <cmath>
#include <QPainter>
#include <QDomDocument>

#include "../Core/GlobalFunc.h"
#include "../Core/View.h"
#include "../Core/GraphicsView.h"
#include "../Image/BaseImage.h"

DiagramLengthItem::DiagramLengthItem()
    : DiagramLineItem(QLineF(), nullptr, nullptr)
{

}

DiagramLengthItem::DiagramLengthItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent)
    : DiagramLineItem(line, contextMenu, parent)
{

}

DiagramLengthItem::~DiagramLengthItem()
{

}

QDomElement DiagramLengthItem::saveToXML(QDomDocument* doc)
{
    QDomElement lineItem = doc->createElement("DiagramItem");
    lineItem.setAttribute("Type", "DiagramLineItem");

    QDomElement attribute = doc->createElement("Attribute");
    attribute.setAttribute("Name", "Length");
    attribute.setAttribute("Point1", QString("%1 %2").arg(line().p1().x()).arg(line().p1().y()));
    attribute.setAttribute("Point2", QString("%1 %2").arg(line().p2().x()).arg(line().p2().y()));
    attribute.setAttribute("Color", QString("%1 %2 %3").arg(pen().color().red()).arg(pen().color().green()).arg(pen().color().blue()));

    lineItem.appendChild(attribute);
    return lineItem;

 /*   QDomElement name = doc->createElement("Name");
    lineItem.appendChild(name);
    QDomText text = doc->createTextNode("Length");
    name.appendChild(text);

    QDomElement point1 = doc->createElement("Point1");
    lineItem.appendChild(point1);
    text = doc->createTextNode(QString("%1 %2").arg(line().p1().x()).arg(line().p1().y()));
    point1.appendChild(text);

    QDomElement point2 = doc->createElement("Point2");
    lineItem.appendChild(point2);
    text = doc->createTextNode(QString("%1 %2").arg(line().p2().x()).arg(line().p2().y()));
    point2.appendChild(text);

    QDomElement color = doc->createElement("Color");
    lineItem.appendChild(color);
    text = doc->createTextNode(QString("%1 %2 %3").arg(pen().color().red()).arg(pen().color().green()).arg(pen().color().blue()));
    color.appendChild(text);*/
}
/*
void DiagramLengthItem::loadFromXML(const QDomElement& e)
{

}*/

void DiagramLengthItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    DiagramLineItem::paint(painter, option, widget);

    drawResizeHandle(painter);
    drawLengthText(painter);
}

QString DiagramLengthItem::lengthString() const
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

void DiagramLengthItem::drawLengthText(QPainter* painter)
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
