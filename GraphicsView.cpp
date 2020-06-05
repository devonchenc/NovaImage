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
	setDragMode(QGraphicsView::NoDrag);

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

void GraphicsView::setZoomValue(int value)
{
	_zoomFactor = value;
	applyZoomValue();
}

void GraphicsView::setZoomValueOffset(int offset)
{
	_zoomFactor += offset;
	emit zoomValueChanged(_zoomFactor);
}

void GraphicsView::zoomNormal()
{
	_zoomFactor = MAX_ZOOM / 2;
	emit zoomValueChanged(_zoomFactor);
}

void GraphicsView::zoom2x()
{
	_zoomFactor = MAX_ZOOM / 2 + ZOOM_STEP;
	emit zoomValueChanged(_zoomFactor);
}

void GraphicsView::zoom4x()
{
	_zoomFactor = MAX_ZOOM / 2 + ZOOM_STEP * 2;
	emit zoomValueChanged(_zoomFactor);
}

void GraphicsView::zoom8x()
{
	_zoomFactor = MAX_ZOOM / 2 + ZOOM_STEP * 3;
	emit zoomValueChanged(_zoomFactor);
}

void GraphicsView::zoomIn()
{
	_zoomFactor += ZOOM_STEP / 10;
	_zoomFactor = qMin(_zoomFactor, MAX_ZOOM);
	emit zoomValueChanged(_zoomFactor);
}

void GraphicsView::zoomOut()
{
	_zoomFactor -= ZOOM_STEP / 10;
	_zoomFactor = qMax(0, _zoomFactor);
	emit zoomValueChanged(_zoomFactor);
}

void GraphicsView::applyZoomValue()
{
	_zoomFactor = qMax(0, _zoomFactor);
	_zoomFactor = qMin(_zoomFactor, MAX_ZOOM);
	qreal scale = qPow(qreal(2), (_zoomFactor - MAX_ZOOM / 2) / qreal(ZOOM_STEP));

	QMatrix matrix;
	matrix.scale(scale, scale);
	setMatrix(matrix);
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
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

	MouseHandler::handleMove(event);

	QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	MouseHandler::handleRelease(event);

	QGraphicsView::mouseReleaseEvent(event);
}