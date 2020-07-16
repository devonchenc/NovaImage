#pragma once

#include "DiagramLineItem.h"

class DiagramPlotItem : public DiagramLineItem
{
	Q_OBJECT

public:
	DiagramPlotItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
	~DiagramPlotItem();

	int plotIndex() { return _plotIndex; }

signals:
	void itemDeleted();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	void drawPlotIndex(QPainter* painter);

private:
	int _plotIndex;
	static int _plotCount;
};
