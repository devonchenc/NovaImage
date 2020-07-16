#include "DiagramPlotItem.h"

#include <QPainter>

#include "../Core/GlobalFunc.h"
#include "../Core/View.h"
#include "../Core/GraphicsView.h"

int DiagramPlotItem::_plotCount = 0;

DiagramPlotItem::DiagramPlotItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent)
	: DiagramLineItem(line, contextMenu, parent)
{
	_plotCount++;
	_plotIndex = _plotCount;
}

DiagramPlotItem::~DiagramPlotItem()
{
	emit itemDeleted();
}

void DiagramPlotItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	DiagramLineItem::paint(painter, option, widget);

	drawResizeHandle(painter);
	drawPlotIndex(painter);
}

void DiagramPlotItem::drawPlotIndex(QPainter* painter)
{
	QTransform transform = getGlobalView()->view()->transform();
	QTransform transform2;
	transform2.translate(line().p2().x() + 10, line().p2().y() + 5);

	painter->setWorldTransform(transform.inverted() * transform2, true);
	painter->setFont(QFont("Arial", 10));
	painter->setPen(QPen(Qt::yellow));
	painter->drawText(0, 0, QString::number(_plotIndex));
}