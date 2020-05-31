#pragma once

#include <QGraphicsScene>
#include "Widget/DiagramItem.h"
#include "Widget/DiagramTextItem.h"

class DiagramLineItem;

#define MOVE_ITEM		0
#define MOVE_SCENE		1
#define SELECT_ITEM		2
#define INSERT_ITEM		3
#define INSERT_TEXT		4
#define INSERT_LINE		5

class GraphicsScene : public QGraphicsScene
{
	Q_OBJECT

public:
	explicit GraphicsScene(QMenu* itemMenu, QObject* parent = nullptr);
	QFont font() const { return _font; }
	QColor textColor() const { return _textColor; }
	QColor itemColor() const { return _fillColor; }
	QColor lineColor() const { return _lineColor; }

	// utilities
	void deleteItems(QList<QGraphicsItem*> const& items);

	void setMode(int mode);

public slots:
	void setItemType(DiagramItem::DiagramType type);
	void editorLostFocus(DiagramTextItem* item);
	void setLineColor(const QColor& color);
	void enableFillColor(bool enable);
	void setFillColor(const QColor& color);
	void setTextColor(const QColor& color);
	void setTextFont(const QFont& font);

	void itemSelectedChange(QGraphicsItem* item);

signals:
	void itemInserted(QGraphicsItem* item);
	void itemSelected(QGraphicsItem* item);
	void textSelected(QGraphicsItem* item);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
	void keyPressEvent(QKeyEvent* keyEvent) override;

private:
	DiagramItem::DiagramType _itemType;
	QMenu* _itemMenu;
	int _mode;
	QPointF _startPoint;
	bool _enableFill;
	QColor _lineColor;
	QColor _fillColor;
	QColor _textColor;
	QFont _font;

	DiagramLineItem* _line;

	DiagramItem* _currentItem;
};