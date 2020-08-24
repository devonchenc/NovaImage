#pragma once

#include "DiagramLineItem.h"

class DiagramImageQualityItem : public DiagramLineItem
{
    Q_OBJECT

public:
    DiagramImageQualityItem();
    DiagramImageQualityItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
    ~DiagramImageQualityItem();

    int width() { return _lineWidth; }

    void setWidth(int lineWidth) { _lineWidth = lineWidth; }

    QDomElement saveToXML(QDomDocument& doc) override;

    void loadFromXML(const QDomElement& e) override;

signals:
    void itemDeleted();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void drawRect(QPainter* painter);

    void drawPlotIndex(QPainter* painter);

private:
    int _lineWidth;
};
