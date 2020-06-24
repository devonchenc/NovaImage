#include "GraphicsScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QTextCursor>
#include <QKeyEvent>
#include <QBrush>

#include "../Widget/DiagramTextItem.h"
#include "../Widget/DiagramLineItem.h"
#include "GlobalFunc.h"
#include "View.h"

#define MIN_SIZE		10

GraphicsScene::GraphicsScene(QMenu* itemMenu, QObject* parent)
	: QGraphicsScene(parent)
	, _itemMenu(itemMenu)
	, _mode(MOVE_ITEM)
	, _itemType(DiagramItem::Rect)
	, _enableFill(false)
	, _lineColor(qRgb(0, 255, 55))
	, _fillColor(Qt::white)
	, _textColor(Qt::green)
	, _currentDrawingLine(nullptr)
	, _currentDrawingItem(nullptr)
	, _refHorzLine(nullptr)
	, _refVertLine(nullptr)
{
	_font.setPointSize(24);
}

void GraphicsScene::setLineColor(const QColor& color)
{
	_lineColor = color;
	foreach (QGraphicsItem* p, selectedItems())
	{
		if (p->type() == DiagramItem::Type)
		{
			DiagramItem* item = qgraphicsitem_cast<DiagramItem*>(p);
			QPen pen = item->pen();
			pen.setColor(color);
			item->setPen(pen);
		}
		else if (p->type() == DiagramLineItem::Type)
		{
			DiagramLineItem* item = qgraphicsitem_cast<DiagramLineItem*>(p);
			QPen pen = item->pen();
			pen.setColor(color);
			item->setPen(pen);
		}
	}
}

void GraphicsScene::enableFillColor(bool enable)
{
	_enableFill = enable;
	foreach (QGraphicsItem* p, selectedItems())
	{
		if (p->type() == DiagramItem::Type)
		{
			DiagramItem* item = qgraphicsitem_cast<DiagramItem*>(p);
			QBrush brush = item->brush();
			brush.setStyle(_enableFill ? Qt::SolidPattern : Qt::NoBrush);
			item->setBrush(brush);
		}
		else if (p->type() == DiagramLineItem::Type)
		{
		//	DiagramLineItem* item = qgraphicsitem_cast<DiagramLineItem*>(p);
		//	item->setPointPen(QPen(color));
		}
	}
}

void GraphicsScene::setFillColor(const QColor& color)
{
	_fillColor = color;
	foreach (QGraphicsItem* p, selectedItems())
	{
		if (p->type() == DiagramItem::Type)
		{
			DiagramItem* item = qgraphicsitem_cast<DiagramItem*>(p);
			QBrush brush(_fillColor);
			brush.setStyle(_enableFill ? Qt::SolidPattern : Qt::NoBrush);
			item->setBrush(brush);
		}
		else if (p->type() == DiagramLineItem::Type)
		{
			DiagramLineItem* item = qgraphicsitem_cast<DiagramLineItem*>(p);
			item->setEndpointPen(QPen(color));
		}
	}
}

void GraphicsScene::setTextColor(const QColor& color)
{
	_textColor = color;
	foreach (QGraphicsItem* p, selectedItems())
	{
		if (p->type() == DiagramTextItem::Type)
		{
			DiagramTextItem* item = qgraphicsitem_cast<DiagramTextItem*>(p);
			item->setDefaultTextColor(_textColor);
		}
	}
}

void GraphicsScene::setTextFont(const QFont& font)
{
	_font = font;
	foreach(QGraphicsItem* p, selectedItems())
	{
		if (p->type() == DiagramTextItem::Type)
		{
			DiagramTextItem* item = qgraphicsitem_cast<DiagramTextItem*>(p);
			int size = _font.pointSize();
			item->setFont(_font);
		}
	}
}

void GraphicsScene::deleteItems(QList<QGraphicsItem*> const& items)
{
	qDebug() << "delete items" << items;

	QList<QGraphicsItem*> diagramItems;
	foreach (QGraphicsItem* item, items)
	{
		diagramItems.append(item);
	}

	foreach (QGraphicsItem* item, diagramItems)
	{
		removeItem(item);
		delete item;
	}
}

void GraphicsScene::setMode(int mode)
{
	_mode = mode;
}

void GraphicsScene::setItemType(DiagramItem::DiagramType type)
{
	_itemType = type;
}

void GraphicsScene::showCrossLine(bool show)
{
	if (show)
	{
		QRectF rect = sceneRect();
		_refHorzLine = addLine(rect.left(), rect.center().y(), rect.right(), rect.center().y(), QPen(Qt::red));
		_refVertLine = addLine(rect.center().x(), rect.top(), rect.center().x(), rect.bottom(), QPen(Qt::red));
	}
	else
	{
		removeItem(_refHorzLine);
		removeItem(_refVertLine);
		delete _refHorzLine;
		delete _refVertLine;
	}
}

void GraphicsScene::showMeasurement(bool show)
{
	QList<QGraphicsItem*> itemList = items();
	for (int i = 0; i < itemList.size(); i++)
	{
		if (itemList.at(i) != getGlobalView()->getPixmapItem())
		{
			itemList.at(i)->setVisible(show);
		}
	}
	update();
}

void GraphicsScene::editorLostFocus(DiagramTextItem* item)
{
	QTextCursor cursor = item->textCursor();
	cursor.clearSelection();
	item->setTextCursor(cursor);

	if (item->toPlainText().isEmpty())
	{
		removeItem(item);
		item->deleteLater();
	}
}

void GraphicsScene::mousePress(const QPointF& point)
{
	_startPoint = point;

	// Clear all selected items
	clearSelection();

	if (_mode == INSERT_ITEM)
	{
		if (_itemType <= DiagramItem::Parallelogram)
		{
			_currentDrawingItem = new DiagramItem(_itemType, _itemMenu);
			_currentDrawingItem->setRectF(QRectF(_startPoint, _startPoint));

			QBrush brush(_fillColor);
			brush.setStyle(_enableFill ? Qt::SolidPattern : Qt::NoBrush);
			_currentDrawingItem->setBrush(brush);
			_currentDrawingItem->setPen(QPen(_lineColor, 2));
			connect(_currentDrawingItem, &DiagramItem::itemSelectedChange, this, &GraphicsScene::itemSelectedChange);
			addItem(_currentDrawingItem);
			emit itemInserted(_currentDrawingItem);
		}
		else if (_itemType == DiagramItem::Text)
		{
			DiagramTextItem* textItem = new DiagramTextItem;
			int size = _font.pointSize();
			textItem->setFont(_font);
			textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
			textItem->setZValue(1000.0);
			connect(textItem, &DiagramTextItem::lostFocus, this, &GraphicsScene::editorLostFocus);
			connect(textItem, &DiagramTextItem::textSelectedChange, this, &GraphicsScene::textSelected);
			addItem(textItem);
			textItem->setDefaultTextColor(_textColor);
			textItem->setPos(_startPoint);
			qDebug() << "text inserted at" << textItem->scenePos();
			emit itemInserted(textItem);
		}
		else if (_itemType == DiagramItem::Line || _itemType == DiagramItem::Arrow)
		{
			int type = _itemType - DiagramItem::Line;
			_currentDrawingLine = new DiagramLineItem(type, QLineF(_startPoint, _startPoint), _itemMenu);
			_currentDrawingLine->setPen(QPen(_lineColor, 2));
			_currentDrawingLine->setEndpointPen(QPen(_fillColor));
			connect(_currentDrawingLine, &DiagramLineItem::itemSelectedChange, this, &GraphicsScene::itemSelectedChange);
			addItem(_currentDrawingLine);
			emit itemInserted(_currentDrawingLine);
		}
	}
}

void GraphicsScene::mouseMove(const QPointF& point)
{
	if ((_itemType == DiagramItem::Line || _itemType == DiagramItem::Arrow) && _currentDrawingLine != nullptr)
	{
		_currentDrawingLine->setLine(QLineF(_currentDrawingLine->line().p1(), point));
	}
	else if (_itemType <= DiagramItem::Parallelogram)
	{
		if (_currentDrawingItem)
		{
			_currentDrawingItem->setRectF(QRectF(_startPoint, point));
			_currentDrawingItem->statisticsInfo();
		}
	}

	update();
}

void GraphicsScene::mouseRelease(const QPointF& point)
{
	if (_itemType <= DiagramItem::Parallelogram)
	{
		if (_currentDrawingItem)
		{
			QRectF rect(_startPoint, point);
			_currentDrawingItem->setRectF(rect);
			if (fabs(rect.width()) < MIN_SIZE && fabs(rect.height()) < MIN_SIZE)
			{
				removeItem(_currentDrawingItem);
				delete _currentDrawingItem;
			}
			else
			{
				_currentDrawingItem->setDrawingFinished(true);
				_currentDrawingItem->setSelected(true);
			}
			_currentDrawingItem = nullptr;
		}
	}
	else if ((_itemType == DiagramItem::Line || _itemType == DiagramItem::Arrow) && _currentDrawingLine != nullptr)
	{
		QRectF rect = _currentDrawingLine->boundingRect();
		if (rect.width() < MIN_SIZE && rect.height() < MIN_SIZE)
		{
			removeItem(_currentDrawingLine);
			delete _currentDrawingLine;
		}
		else
		{
			_currentDrawingLine->setDrawingFinished(true);
			_currentDrawingLine->setSelected(true);
		}
		_currentDrawingLine = nullptr;
	}

	update();
}

void GraphicsScene::keyPressEvent(QKeyEvent* keyEvent)
{
	if (keyEvent->key() == Qt::Key_Delete)
	{
		foreach (QGraphicsItem* item, selectedItems())
		{
			removeItem(item);
			delete item;
		}
		update();
	}

	QGraphicsScene::keyPressEvent(keyEvent);
}

void GraphicsScene::itemSelectedChange(QGraphicsItem* item)
{
	if (item->type() == DiagramItem::Type)
	{
		DiagramItem* diagramitem = qgraphicsitem_cast<DiagramItem*>(item);
		_lineColor = diagramitem->pen().color();
		_fillColor = diagramitem->brush().color();
	}
	else if (item->type() == DiagramLineItem::Type)
	{
		DiagramLineItem* lineitem = qgraphicsitem_cast<DiagramLineItem*>(item);
		_lineColor = lineitem->pen().color();
		_fillColor = lineitem->pointPen().color();
	}

	emit itemSelected(item);
}