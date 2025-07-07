#include "DiagramLengthItem.h"

#include <cmath>
#include <QPainter>
#include <QDomDocument>
#include <QGraphicsOpacityEffect>

#include "../Core/GlobalFunc.h"
#include "../Core/View.h"
#include "../Core/GraphicsView.h"
#include "../Image/BaseImage.h"

DiagramLengthItem::DiagramLengthItem(QGraphicsItem* parent)
    : DiagramLineItem(QLineF(), nullptr, parent)
{
}

DiagramLengthItem::DiagramLengthItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent)
    : DiagramLineItem(line, contextMenu, parent)
{
}

QDomElement DiagramLengthItem::saveToXML(QDomDocument& doc) const
{
    QDomElement lineItem = doc.createElement("GraphicsItem");
    lineItem.setAttribute("Type", "DiagramLineItem");

    QDomElement attribute = doc.createElement("Attribute");
    attribute.setAttribute("Name", "Length");
    attribute.setAttribute("Position", pointFToString(pos()));
    attribute.setAttribute("Point1", pointFToString(line().p1()));
    attribute.setAttribute("Point2", pointFToString(line().p2()));
    attribute.setAttribute("Color", colorToString(pen().color()));
    attribute.setAttribute("EndPointColor", colorToString(_endpointPen.color()));
    attribute.setAttribute("Opacity", QString::number(_effect->opacity(), 'f', 2));

    lineItem.appendChild(attribute);
    return lineItem;
}

float DiagramLengthItem::pixelLength() const
{
    float offsetX = line().p1().x() - line().p2().x();
    float offsetY = line().p1().y() - line().p2().y();
    return sqrt(offsetX * offsetX + offsetY * offsetY);
}

std::optional<float> DiagramLengthItem::actualLength() const
{
    BaseImage* image = getGlobalImage();
    if (!image || !image->hasPixelSpacing())
        return std::nullopt;

    float offsetX = line().p1().x() - line().p2().x();
    float offsetY = line().p1().y() - line().p2().y();
    float horzPixelSpacing = image->horzPixelSpacing();
    float vertPixelSpacing = image->vertPixelSpacing();
    offsetX *= horzPixelSpacing;
    offsetY *= vertPixelSpacing;

    std::optional<float> length = sqrt(offsetX * offsetX + offsetY * offsetY);
    return length;
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

void DiagramLengthItem::drawLengthText(QPainter* painter) const
{
    QGraphicsScene* graphicsScene = scene();
    QList<QGraphicsView*> viewList = graphicsScene->views();
    Q_ASSERT(viewList.size() > 0);

    QTransform transform = viewList.first()->transform();
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
