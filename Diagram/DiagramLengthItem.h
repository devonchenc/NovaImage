#pragma once

#include "DiagramLineItem.h"

class DiagramLengthItem : public DiagramLineItem
{
    Q_OBJECT

public:
    DiagramLengthItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
    ~DiagramLengthItem();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    QString lengthString() const;

    void drawLengthText(QPainter* painter);
};
