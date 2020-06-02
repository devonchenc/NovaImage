#include "GraphicsView.h"

#include <QtMath>
#include <QDebug>

#include "View.h"
#include "MouseHandler.h"

GraphicsView::GraphicsView(View* view, QGraphicsScene* scene, QWidget* parent)
	: QGraphicsView(scene, parent)
	, _view(view)
	, _zoomFactor(MAX_ZOOM / 2)
	, _isLBtnDown(false)
{
	setDragMode(QGraphicsView::NoDrag);//ScrollHandDrag
//	setDragMode(QGraphicsView::ScrollHandDrag);

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setMouseTracking(true);
}

#if QT_CONFIG(wheelevent)
void GraphicsView::wheelEvent(QWheelEvent* e)
{
	if (e->modifiers() & Qt::ControlModifier)
	{
		if (e->angleDelta().y() > 0)
			zoomIn();
		else
			zoomOut();

		e->accept();
	}
	else
	{
		QGraphicsView::wheelEvent(e);
	}
}
#endif

void GraphicsView::setValue(int value)
{
	_zoomFactor = value;
	applyValue();
}

void GraphicsView::zoomNormal()
{
	_zoomFactor = MAX_ZOOM / 2;
	emit valueChanged(_zoomFactor);
}

void GraphicsView::zoom2x()
{
	_zoomFactor = MAX_ZOOM / 2 + 50;
	emit valueChanged(_zoomFactor);
}

void GraphicsView::zoom4x()
{
	_zoomFactor = MAX_ZOOM / 2 + 100;
	emit valueChanged(_zoomFactor);
}

void GraphicsView::zoom8x()
{
	_zoomFactor = MAX_ZOOM / 2 + 150;
	emit valueChanged(_zoomFactor);
}

void GraphicsView::zoomIn()
{
	_zoomFactor += 5;
	_zoomFactor = qMin(_zoomFactor, MAX_ZOOM);
	emit valueChanged(_zoomFactor);
}

void GraphicsView::zoomOut()
{
	_zoomFactor -= 5;
	_zoomFactor = qMax(0, _zoomFactor);
	emit valueChanged(_zoomFactor);
}

void GraphicsView::applyValue()
{
	_zoomFactor = qMax(0, _zoomFactor);
	_zoomFactor = qMin(_zoomFactor, MAX_ZOOM);
	qreal scale = qPow(qreal(2), (_zoomFactor - MAX_ZOOM / 2) / qreal(50));

	QMatrix matrix;
	matrix.scale(scale, scale);
	setMatrix(matrix);
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		_mouseLBtnDown = event->pos();
		_isLBtnDown = true;
	}

	MouseHandler::handlePress(event);

	QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	QGraphicsPixmapItem* pixmapItem = _view->getPixmapItem();
	if (pixmapItem)
	{
		QPointF pointScene = mapToScene(event->pos());
		QPointF pointPixemap = pixmapItem->mapFromScene(pointScene);

		QString strCoord = QString("x:%1, y:%2").arg(QString::number(pointPixemap.x(), 'f', 0)).arg(QString::number(pointPixemap.y(), 'f', 0));
		emit showCoordinate(strCoord);

		QString strValue;
		QImage image = pixmapItem->pixmap().toImage();
		if (pointPixemap.x() >= 0 && pointPixemap.x() < image.width() && pointPixemap.y() >= 0 && pointPixemap.y() < image.height())
		{
			QRgb value = image.pixel(QPoint(pointPixemap.x(), pointPixemap.y()));
			strValue = QString("R:%1, G:%2, B:%3").arg(qRed(value), 3).arg(qGreen(value), 3).arg(qBlue(value), 3);
		}
		emit showPixelValue(strValue);
	}

//	if (_isLBtnDown)
	if (event->buttons() & Qt::LeftButton)
	{
		QPointF delta = mapToScene(_mouseLBtnDown) - mapToScene(event->pos());
		_mouseLBtnDown = event->pos();

		setSceneRect(sceneRect().x() + delta.x(), sceneRect().y() + delta.y(), sceneRect().width(), sceneRect().height());
	//	centerOn(getCenter() + delta);
	}

	// TODO 写个类来封装鼠标事件处理
	if (event->buttons() & Qt::LeftButton)
	{
		// 调用和鼠标左键绑定的功能
	}
	else if (event->buttons() & Qt::RightButton)
	{
		// 调用和鼠标右键绑定的功能
	}

	QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		_isLBtnDown = false;
	}

	QGraphicsView::mouseReleaseEvent(event);
}