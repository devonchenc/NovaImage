#include "DiagramTextItem.h"

#include <QDebug>
#include <QTextCursor>

#include "../Core/GlobalFunc.h"

DiagramTextItem::DiagramTextItem(QGraphicsItem* parent)
    : QGraphicsTextItem(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    _positionLastTime = QPointF(0, 0);
}

DiagramTextItem* DiagramTextItem::clone()
{
    DiagramTextItem* cloned = new DiagramTextItem(nullptr);
    cloned->setPlainText(toPlainText());
    cloned->setFont(font());
    cloned->setTextWidth(textWidth());
    cloned->setDefaultTextColor(defaultTextColor());
    cloned->setPos(scenePos());
    cloned->setZValue(zValue());
    return cloned;
}

QDomElement DiagramTextItem::saveToXML(QDomDocument& doc)
{
    QDomElement lineItem = doc.createElement("GraphicsItem");
    lineItem.setAttribute("Type", "DiagramTextItem");

    QDomElement attribute = doc.createElement("Attribute");
    attribute.setAttribute("Text", toPlainText());
    attribute.setAttribute("Position", pointFToString(pos()));
    attribute.setAttribute("DefaultTextColor", colorToString(defaultTextColor()));
    attribute.setAttribute("Font", font().family());
    attribute.setAttribute("PointSize", QString::number(font().pointSize()));
    attribute.setAttribute("Weight", QString::number(font().weight()));
    attribute.setAttribute("Italic", QString::number(font().italic()));
    attribute.setAttribute("Underline", QString::number(font().underline()));

    lineItem.appendChild(attribute);
    return lineItem;
}

void DiagramTextItem::loadFromXML(const QDomElement& e)
{
    setPlainText(e.attribute("Text"));
    setPos(stringToPointF(e.attribute("Position")));
    setDefaultTextColor(stringToColor(e.attribute("DefaultTextColor")));

    QFont font = this->font();
    font.setFamily(e.attribute("Font"));
    font.setPointSize(e.attribute("PointSize").toInt());
    font.setWeight(e.attribute("Weight").toInt());
    font.setItalic(e.attribute("Italic").toInt());
    font.setUnderline(e.attribute("Underline").toInt());
    setFont(font);
}

QVariant DiagramTextItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        emit textSelectedChange(this);
    return value;
}

void DiagramTextItem::focusInEvent(QFocusEvent* event)
{
    if (_positionLastTime == QPointF(0, 0))
        // initialize positionLastTime to insertion position
        _positionLastTime = scenePos();
    QGraphicsTextItem::focusInEvent(event);
}

void DiagramTextItem::focusOutEvent(QFocusEvent* event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    if (_contentLastTime == toPlainText())
    {
        _contentHasChanged = false;
    }
    else
    {
        _contentLastTime = toPlainText();
        _contentHasChanged = true;
    }
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}

void DiagramTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction)
    {
        setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void DiagramTextItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsTextItem::mousePressEvent(event);
}

void DiagramTextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (scenePos() != _positionLastTime)
    {
        qDebug() << scenePos() << "::" << _positionLastTime;
    }
    _positionLastTime = scenePos();
    QGraphicsTextItem::mouseReleaseEvent(event);
}
