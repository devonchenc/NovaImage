#pragma once

#include "DiagramLineItem.h"

class DiagramLengthItem : public DiagramLineItem
{
    Q_OBJECT

public:
    DiagramLengthItem(QGraphicsItem* parent = nullptr);
    DiagramLengthItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);

    QDomElement saveToXML(QDomDocument& doc) override;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    QString lengthString() const;

    void drawLengthText(QPainter* painter);
};
