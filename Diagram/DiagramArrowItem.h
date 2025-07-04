#pragma once

#include "DiagramLineItem.h"

class DiagramArrowItem : public DiagramLineItem
{
    Q_OBJECT

public:
    DiagramArrowItem(QGraphicsItem* parent = nullptr);
    DiagramArrowItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);

    QDomElement saveToXML(QDomDocument& doc) const override;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    void drawArrow(QPainter* painter);
};
