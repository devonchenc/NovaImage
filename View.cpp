#include "View.h"

#include <QVBoxLayout>
#include <QGraphicsItem>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QDebug>

#include "GraphicsView.h"
#include "GraphicsScene.h"

View::View(QWidget* parent)
	: QFrame(parent)
	, _currentImage(nullptr)
{
	createItemMenus();

	_scene = new GraphicsScene(_itemMenu, parent);
	_view = new GraphicsView(this, _scene);
	setMouseTracking(true);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(_view);

	setLayout(layout);
}

GraphicsView* View::view() const
{
	return _view;
}

GraphicsScene* View::scene()
{
	return _scene;
}

void View::createItemMenus()
{
	_cutAction = new QAction(QIcon("cut.png"), tr("Cu&t"), this);
	_cutAction->setShortcut(tr("Ctrl+X"));
	connect(_cutAction, &QAction::triggered, this, &View::cutItem);

	_copyAction = new QAction(QIcon("copy.png"), tr("&Copy"), this);
	_copyAction->setShortcut(tr("Ctrl+C"));
	connect(_copyAction, &QAction::triggered, this, &View::copyItem);

	_pasteAction = new QAction(QIcon("paste.png"), tr("&Paste"), this);
	_pasteAction->setShortcut(tr("Ctrl+V"));
	connect(_pasteAction, &QAction::triggered, this, &View::pasteItem);

	_deleteAction = new QAction(QIcon("delete.png"), tr("&Delete"), this);
	_deleteAction->setShortcut(tr("Delete"));
	connect(_deleteAction, &QAction::triggered, this, &View::deleteItem);

	_itemMenu = new QMenu;
	_itemMenu->addAction(_cutAction);
	_itemMenu->addAction(_copyAction);
	_itemMenu->addAction(_pasteAction);
	_itemMenu->addSeparator();
	_itemMenu->addAction(_deleteAction);
}

void View::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		_cutAction->setText(tr("Cu&t"));
		_copyAction->setText(tr("&Copy"));
		_pasteAction->setText(tr("&Paste"));
		_deleteAction->setText(tr("&Delete"));
	}

	QFrame::changeEvent(event);
}

void View::showImage(const QPixmap& image)
{
	if (_currentImage)
	{
		_scene->clear();
		_currentImage = nullptr;
	}

	_scene->clear();
	_view->resetMatrix();
	_currentImage = _scene->addPixmap(image);
	_currentImage->setZValue(-1.0);
	_scene->update();
	_view->setSceneRect(image.rect());

	QString strInfo = QString(tr("Image Size:%1 x %2")).arg(image.width(), 4).arg(image.height(), 4);
	emit showInfo(strInfo);
}

void View::showImage(const QImage* image)
{
	if (_currentImage)
	{
		_scene->clear();
		_currentImage = nullptr;
	}

	_view->resetMatrix();
	QPixmap pixmap = QPixmap::fromImage(*image);
	_currentImage = _scene->addPixmap(pixmap);
	_currentImage->setZValue(-1.0);
	_scene->update();
	_view->setSceneRect(pixmap.rect());

	QString strInfo = QString(tr("Image Size:%1 x %2")).arg(image->width(), 4).arg(image->height(), 4);
	emit showInfo(strInfo);
}

void View::resetImage()
{
	if (_currentImage)
	{
		_scene->clear();
		_currentImage = nullptr;
	}

	_view->resetMatrix();
	_scene->update();
}

void View::setSceneMode(int mode)
{
	if (mode == MOVE_SCENE)
	{
		_view->setDragMode(QGraphicsView::ScrollHandDrag);
	}
	else if (mode == MOVE_ITEM)
	{
		_view->setDragMode(QGraphicsView::RubberBandDrag);
	}

	_scene->setMode(mode);
}

void View::setItemType(DiagramItem::DiagramType type)
{
	_scene->setItemType(type);
}

void View::fitWindow()
{
	QRectF rect = this->rect();
	qreal imageWidth = _currentImage->pixmap().width();
	qreal imageHeight = _currentImage->pixmap().height();
	
	qreal ratio = qMin(rect.width() / imageWidth, rect.height() / imageHeight);

	int value = log2(ratio) * 50 + MAX_ZOOM / 2;
	_view->setValue(value);
	emit _view->valueChanged(value);
}

void View::zoomNormal()
{
	view()->zoomNormal();
}

void View::zoom2x()
{
	view()->zoom2x();
}

void View::zoom4x()
{
	view()->zoom4x();
}

void View::zoom8x()
{
	view()->zoom8x();
}

void View::zoomIn()
{
	view()->zoomIn();
}

void View::zoomOut()
{
	view()->zoomOut();
}

void View::cutItem()
{
	copyItem();
	deleteItem();
}

void View::copyItem()
{
	foreach (QGraphicsItem* p, pasteBoard)
	{
		delete p;
	}
	pasteBoard = cloneItems(_scene->selectedItems());
}

void View::pasteItem()
{
	QList<QGraphicsItem*> pasteBoardCopy(cloneItems(pasteBoard));
	foreach (QGraphicsItem* p, _scene->items())
		p->setSelected(false);

	foreach (QGraphicsItem* item, pasteBoard)
	{
		item->setPos(item->scenePos() + QPointF(20, 20));
		item->setZValue(item->zValue() + 0.1);  // raise a little bit
		_scene->addItem(item);
		item->setSelected(true);
	}
	pasteBoard.swap(pasteBoardCopy);
}

void View::deleteItem()
{
	_scene->deleteItems(_scene->selectedItems());
}

QList<QGraphicsItem*> View::cloneItems(const QList<QGraphicsItem*>& items)
{
	QHash<QGraphicsItem*, QGraphicsItem*> copyMap;
	foreach (QGraphicsItem* item, items)
	{
		if (item->type() == DiagramItem::Type)
		{
			copyMap[item] = qgraphicsitem_cast<DiagramItem*>(item)->clone();
		}
		else if (item->type() == DiagramTextItem::Type)
		{
			copyMap[item] = qgraphicsitem_cast<DiagramTextItem*>(item)->clone();
		}
	}

	return copyMap.values();
}