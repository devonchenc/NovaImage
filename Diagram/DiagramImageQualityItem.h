#pragma once

#include "DiagramLineItem.h"

class DiagramImageQualityItem : public DiagramLineItem
{
    Q_OBJECT

public:
    DiagramImageQualityItem(QGraphicsItem* parent = nullptr);
    DiagramImageQualityItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
    ~DiagramImageQualityItem();

    int width() { return _lineWidth; }

    void setWidth(int lineWidth) { _lineWidth = lineWidth; }

    QDomElement saveToXML(QDomDocument& doc) override;

    void loadFromXML(const QDomElement& e) override;

    float leftRate() { return _leftRate; }
    float rightRate() { return _rightRate; }

    void setLeftRate(float rate) { _leftRate = rate; }
    void setRightRate(float rate) { _rightRate = rate; }

signals:
    void itemDeleted();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void drawRect(QPainter* painter);

private:
    int _lineWidth;

    float _leftRate, _rightRate;
};
