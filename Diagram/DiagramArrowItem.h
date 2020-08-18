#pragma once

#include "DiagramLineItem.h"

class DiagramArrowItem : public DiagramLineItem
{
    Q_OBJECT

public:
    DiagramArrowItem();
    DiagramArrowItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
    ~DiagramArrowItem();

    QDomElement saveToXML(QDomDocument& doc) override;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    void drawArrow(QPainter* painter);
};
