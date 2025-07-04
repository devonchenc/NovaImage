#pragma once

#include "DiagramLineItem.h"

class DiagramPlotItem : public DiagramLineItem
{
    Q_OBJECT

public:
    DiagramPlotItem(QGraphicsItem* parent = nullptr);
    DiagramPlotItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
    ~DiagramPlotItem();

    int width() const { return _lineWidth; }

    void setWidth(int lineWidth) { _lineWidth = lineWidth; }

    int plotIndex() const { return _plotIndex; }

    QDomElement saveToXML(QDomDocument& doc) const override;

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

    int _plotIndex;
    static int _plotCount;
};
