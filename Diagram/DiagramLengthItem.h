#pragma once

#include <optional>

#include "DiagramLineItem.h"

class DiagramLengthItem : public DiagramLineItem
{
    Q_OBJECT

public:
    DiagramLengthItem(QGraphicsItem* parent = nullptr);
    DiagramLengthItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);

    QDomElement saveToXML(QDomDocument& doc) const override;

    float pixelLength() const;

    std::optional<float> actualLength() const;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    QString lengthString() const;

    void drawLengthText(QPainter* painter);
};
