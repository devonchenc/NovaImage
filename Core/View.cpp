#include "View.h"

#include <QVBoxLayout>
#include <QGraphicsItem>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QDebug>

#include "GraphicsView.h"
#include "GraphicsScene.h"
#include "../Processor/LevelsProcessor.h"
#include "../Widget/PlotDialog.h"
#include "GlobalFunc.h"
#include "Document.h"
#include "../Image/BaseImage.h"

View::View(QWidget* parent)
	: QFrame(parent)
	, _currentImage(nullptr)
	, _windowWidth(0)
	, _windowLevel(0)
	, _plotDlg(nullptr)
{
	createItemMenus();

	_scene = new GraphicsScene(_itemMenu, parent);
	_view = new GraphicsView(this, _scene);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(_view);

	setLayout(layout);

	setStyleSheet("background-color:black");
}

View::~View()
{
	if (_plotDlg)
	{
		delete _plotDlg;
		_plotDlg = nullptr;
	}
}

GraphicsView* View::view() const
{
	return _view;
}

GraphicsScene* View::scene()
{
	return _scene;
}

int View::sceneMode()
{
	return _scene->mode();
}

void View::createItemMenus()
{
	_cutAction = new QAction(QIcon("Resources/cut.png"), tr("Cu&t"), this);
	_cutAction->setShortcut(QKeySequence(Qt::Key_X));
	connect(_cutAction, &QAction::triggered, this, &View::cutItem);

	_copyAction = new QAction(QIcon("Resources/copy.png"), tr("&Copy"), this);
	_copyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
	connect(_copyAction, &QAction::triggered, this, &View::copyItem);

	_pasteAction = new QAction(QIcon("Resources/paste.png"), tr("&Paste"), this);
	_pasteAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
	connect(_pasteAction, &QAction::triggered, this, &View::pasteItem);

	_deleteAction = new QAction(QIcon("Resources/delete.png"), tr("&Delete"), this);
	_deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));
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

void View::showImage(const QImage* image, bool resetMatrix)
{
	if (_currentImage)
	{
		if (resetMatrix)
		{
			_scene->clear();
			_currentImage = nullptr;
		}
		else
		{
			_scene->removeItem(_currentImage);
			delete _currentImage;
		}
	}

	QPixmap pixmap = QPixmap::fromImage(*image);
	_currentImage = _scene->addPixmap(pixmap);
	_currentImage->setZValue(-1.0);
	_scene->update();

	if (resetMatrix)
	{
		_view->resetMatrix();
		_view->setSceneRect(pixmap.rect());
	}
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

void View::setWindowWidthAndLevel(float windowWidth, float windowLevel)
{
	_windowWidth = windowWidth;
	_windowLevel = windowLevel;

	getGlobalDocument()->applyImageWidthAndLevel();
}

void View::showPlotDialog(QGraphicsLineItem* lineItem)
{
	if (getGlobalImage() == nullptr)
		return;
	
	if (_plotDlg == nullptr)
	{
		_plotDlg = new PlotDialog(this);
	}

	QPointF p1 = lineItem->line().p1();
	QPointF p2 = lineItem->line().p2();
	qreal distance = sqrt((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()));
	QPointF slope = lineItem->line().p2() - lineItem->line().p1();
	QPointF orthoSlope(-slope.y(), slope.x());
	if (orthoSlope.x() != 0.0f || orthoSlope.y() != 0.0f)
	{
		orthoSlope /= sqrt(orthoSlope.x() * orthoSlope.x() + orthoSlope.y() * orthoSlope.y());
	}

	int lineWidth = 1;

	QVector<qreal> dataVec;
	for (qreal i = 0; i <= distance; i++)
	{
		qreal average = 0.0f;
		int count = 0;
		for (int n = 0; n < lineWidth; n++)
		{
			// 计算线宽度带来的垂直方向偏移
			float offset = (-lineWidth + 1) / 2.0f + n;
			QPointF temp = p1 + orthoSlope * offset + slope / distance * i;
			float fValue = getGlobalImage()->getValue(float(temp.x()), float(temp.y()));
			if (fValue != -1.0f)
			{
				average += fValue;
				count++;
			}
		}

		if (count > 0)
		{
			average /= count;
		}
		else
		{
			average = -1.0;
		}
		dataVec.push_back(average);
	}

	_plotDlg->setData(lineItem, dataVec);
	_plotDlg->show();
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
	else
	{
		_view->setDragMode(QGraphicsView::NoDrag);
	}

	_scene->setMode(mode);
}

void View::setItemType(DiagramItem::DiagramType type)
{
	_scene->setItemType(type);
}

void View::flipHorizontal()
{
	QTransform transform = _view->transform();
	transform *= QTransform(-1, 0, 0, 1, 0, 0);
	_view->setTransform(transform);
}

void View::flipVertical()
{
	QTransform transform = _view->transform();
	transform *= QTransform(1, 0, 0, -1, 0, 0);
	_view->setTransform(transform);
}

void View::rotate90CW()
{
	_view->rotate(90);
}

void View::rotate90CCW()
{
	_view->rotate(-90);
}

void View::rotate180()
{
	_view->rotate(180);
}

void View::resetTransformation()
{
	// Reset rotation
	_view->resetTransform();
	// Reset translate
	if (_currentImage)
	{
		view()->setSceneRect(QRectF(0, 0, _currentImage->pixmap().width(), _currentImage->pixmap().height()));
	}
}

void View::fitWindow()
{
	if (_currentImage)
	{
		QRectF rect = this->rect();
		qreal imageWidth = _currentImage->pixmap().width();
		qreal imageHeight = _currentImage->pixmap().height();

		qreal ratio = qMin(rect.width() / imageWidth, rect.height() / imageHeight);

		int value = log2(ratio) * ZOOM_STEP + MAX_ZOOM / 2;
		_view->setZoomValue(value);
	}
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