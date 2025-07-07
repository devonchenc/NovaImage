#pragma once

#include "DiagramLineItem.h"

class DiagramImageQualityItem : public DiagramLineItem
{
    Q_OBJECT

public:
    DiagramImageQualityItem(QGraphicsItem* parent = nullptr);
    DiagramImageQualityItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
    ~DiagramImageQualityItem();

    int width() const { return _lineWidth; }

    void setWidth(int lineWidth) { _lineWidth = lineWidth; }

    QDomElement saveToXML(QDomDocument& doc) const override;

    void loadFromXML(const QDomElement& e) override;

    float leftRatio() const { return _leftRatio; }
    float rightRatio() const { return _rightRatio; }

public slots:
    void setLeftRatio(float ratio);
    void setRightRatio(float ratio);

signals:
    void itemDeleted();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void drawRect(QPainter* painter) const;

private:
    int _lineWidth;

    float _leftRatio, _rightRatio;
};
