#include "DiagramTextItem.h"

#include <QDebug>
#include <QTextCursor>

DiagramTextItem::DiagramTextItem(QGraphicsItem* parent)
    : QGraphicsTextItem(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    positionLastTime = QPointF(0, 0);
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

QVariant DiagramTextItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        emit textSelectedChange(this);
    return value;
}

void DiagramTextItem::focusInEvent(QFocusEvent* event)
{
    if (positionLastTime == QPointF(0, 0))
        // initialize positionLastTime to insertion position
        positionLastTime = scenePos();
    QGraphicsTextItem::focusInEvent(event);
}

void DiagramTextItem::focusOutEvent(QFocusEvent* event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    if (contentLastTime == toPlainText())
    {
        contentHasChanged = false;
    }
    else
    {
        contentLastTime = toPlainText();
        contentHasChanged = true;
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
    if (scenePos() != positionLastTime)
    {
        qDebug() << scenePos() << "::" << positionLastTime;
    }
    positionLastTime = scenePos();
    QGraphicsTextItem::mouseReleaseEvent(event);
}
