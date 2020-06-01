#include "GraphicsScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QTextCursor>
#include <QKeyEvent>
#include <QBrush>
#include "Widget/DiagramTextItem.h"
#include "Widget/DiagramLineItem.h"

GraphicsScene::GraphicsScene(QMenu* itemMenu, QObject* parent)
	: QGraphicsScene(parent)
	, _itemMenu(itemMenu)
	, _mode(MOVE_ITEM)
	, _itemType(DiagramItem::Rect)
	, _enableFill(false)
	, _lineColor(Qt::blue)
	, _fillColor(Qt::white)
	, _textColor(Qt::green)
	, _line(nullptr)
	, _currentItem(nullptr)
	, _showCrossLine(false)
	, _refHorzLine(nullptr)
	, _refVertLine(nullptr)
{
	_font.setPointSize(16);
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
			item->setPointPen(QPen(color));
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

void GraphicsScene::showCrossLine()
{
	if (_showCrossLine)
	{
		_showCrossLine = false;
		removeItem(_refHorzLine);
		removeItem(_refVertLine);
	}
	else
	{
		_showCrossLine = true;
		QRectF rect = sceneRect();
		_refHorzLine = addLine(rect.left(), rect.center().y(), rect.right(), rect.center().y(), QPen(Qt::red));
		_refVertLine = addLine(rect.center().x(), rect.top(), rect.center().x(), rect.bottom(), QPen(Qt::red));
	}
}

void GraphicsScene::setItemType(DiagramItem::DiagramType type)
{
	_itemType = type;
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

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	if (mouseEvent->button() != Qt::LeftButton)
		return;

	_startPoint = mouseEvent->scenePos();

	DiagramItem* item;
	DiagramTextItem* textItem;
	QGraphicsItem* p = nullptr;
	int itemtype = 0;
	switch (_mode)
	{
	case MOVE_ITEM:
		break;
	case MOVE_SCENE:
	//	qDebug() << "now: " << mouseEvent->scenePos();
		break;
	case INSERT_ITEM:
		if (_itemType <= DiagramItem::Parallelogram)
		{
			item = new DiagramItem(_itemType, _itemMenu);

			_currentItem = item;
			item->setRectF(QRectF(_startPoint, _startPoint));

			QBrush brush(_fillColor);
			brush.setStyle(_enableFill ? Qt::SolidPattern : Qt::NoBrush);
			item->setBrush(brush);
			item->setPen(QPen(_lineColor, 2));
			connect(item, &DiagramItem::itemSelectedChange, this, &GraphicsScene::itemSelectedChange);
			addItem(item);
			emit itemInserted(item);
		}
		else if (_itemType == DiagramItem::Text)
		{
			textItem = new DiagramTextItem;
			int size = _font.pointSize();
			textItem->setFont(_font);
			textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
			textItem->setZValue(1000.0);
			connect(textItem, &DiagramTextItem::lostFocus, this, &GraphicsScene::editorLostFocus);
			connect(textItem, &DiagramTextItem::textSelectedChange, this, &GraphicsScene::textSelected);
			addItem(textItem);
			textItem->setDefaultTextColor(_textColor);
			textItem->setPos(mouseEvent->scenePos());
			qDebug() << "text inserted at" << textItem->scenePos();
			emit itemInserted(textItem);
		}
		else if (_itemType == DiagramItem::Line)
		{
			_line = new DiagramLineItem(QLineF(mouseEvent->scenePos(), mouseEvent->scenePos()), _itemMenu);
			_line->setPen(QPen(_lineColor, 2));
			_line->setPointPen(QPen(_fillColor, 2));
			_line->setFlag(QGraphicsItem::ItemIsMovable, true);
			_line->setFlag(QGraphicsItem::ItemIsSelectable, true);
			connect(_line, &DiagramLineItem::itemSelectedChange, this, &GraphicsScene::itemSelectedChange);
			addItem(_line);
			emit itemInserted(_line);
		}
		setMode(MOVE_ITEM);
		break;
	}
	QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{

	if (_itemType == DiagramItem::Line && _line != nullptr)
	{
		QLineF newLine(_line->line().p1(), mouseEvent->scenePos());
		_line->setLine(newLine);
	}
	else if (_itemType <= DiagramItem::Parallelogram)
	{
	//	QList<QGraphicsItem*> list = selectedItems();
	//	for (int i = 0; i < list.size(); i++)
	//	{
	//		QGraphicsItem* item = list.at(i);
	//		DiagramItem* lineItem = static_cast<DiagramItem*>(item);
	//		lineItem->setRectF(QRectF(_startPoint, end));
	//		qDebug() << "Rect: " << _startPoint << end;
		//	item->setRect(QRect(_startPoint, end));
	//	}
		// For test
		if (_currentItem)
		{
			QPointF end = mouseEvent->scenePos();
			_currentItem->setRectF(QRectF(_startPoint, end));
			qDebug() << "Rect: " << _startPoint << end;
		}
	}
	else if (_mode == MOVE_SCENE)
	{
	/*	QPointF disPointF = mouseEvent->scenePos() - _startPoint;
		qDebug() << "now: " << mouseEvent->scenePos() << "  start: " << _startPoint;
		_startPoint = mouseEvent->scenePos();
		setSceneRect(sceneRect().x() + disPointF.x(), sceneRect().y() + disPointF.y(), sceneRect().width(), sceneRect().height());
		update();*/
	}

	QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	foreach (QGraphicsItem* p, selectedItems())
	{
		p->setFlag(QGraphicsItem::ItemIsMovable);
	}

	// For test
	if (_itemType <= DiagramItem::Parallelogram)
	{
		if (_currentItem)
		{
			QPointF end = mouseEvent->scenePos();
			_currentItem->setRectF(QRectF(_startPoint, end));
			_currentItem = nullptr;
		}
	}


	if (_itemType == DiagramItem::Line && _line != nullptr)
	{
		QRectF rect = _line->boundingRect();
		if (rect.width() < 10 && rect.height() < 10)
		{
			removeItem(_line);
			delete _line;
		}
		_line = nullptr;
	}

	QGraphicsScene::mouseReleaseEvent(mouseEvent);
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