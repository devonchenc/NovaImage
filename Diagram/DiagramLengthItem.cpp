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
    QDomElement lineItem = doc->createElement("GraphicsItem");
    lineItem.setAttribute("Type", "DiagramLineItem");

    QDomElement attribute = doc->createElement("Attribute");
    attribute.setAttribute("Name", "Length");
    attribute.setAttribute("Point1", pointFToString(line().p1()));
    attribute.setAttribute("Point2", pointFToString(line().p2()));
    attribute.setAttribute("Color", colorToString(pen().color()));
    attribute.setAttribute("EndPointColor", colorToString(_endpointPen.color()));

    lineItem.appendChild(attribute);
    return lineItem;
}

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
