#pragma once

#include <QFrame>
#include <QGraphicsScene>

class DiagramItem;
class GraphicsView;
#include "GraphicsScene.h"

class View : public QFrame
{
	Q_OBJECT

public:
	explicit View(QWidget* parent = nullptr);

	GraphicsView* view() const;

	GraphicsScene* scene();

	void showImage(const QPixmap& image);
	void showImage(const QImage* image);

	QGraphicsPixmapItem* getPixmapItem() { return _currentImage; }

	void resetImage();

signals:
	void showInfo(const QString& str);

public slots:
	void setSceneMode(int mode);

	void fitScreen();

private slots:
	void cutItem();
	void copyItem();
	void pasteItem();
	void deleteItem();

protected:
	void changeEvent(QEvent* event) override;

private:
	void createItemMenus();

	QList<QGraphicsItem*> cloneItems(QList<QGraphicsItem*> const& items);

private:
	GraphicsScene* _scene;
	GraphicsView* _view;

	QGraphicsPixmapItem* _currentImage;

	QMenu* _itemMenu;
	QAction* _copyAction;
	QAction* _pasteAction;
	QAction* _cutAction;
	QAction* _deleteAction;

	QList<QGraphicsItem*> pasteBoard;
};