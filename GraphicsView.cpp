#include "GraphicsView.h"

#include <QtMath>
#include <QDebug>

#include "View.h"
#include "MouseHandler.h"
#include "GlobalFunc.h"
#include "Image/BaseImage.h"

GraphicsView::GraphicsView(View* view, QGraphicsScene* scene, QWidget* parent)
	: QGraphicsView(scene, parent)
	, _view(view)
	, _zoomFactor(MAX_ZOOM / 2)
	, _showAnnotation(true)
{
	setDragMode(QGraphicsView::NoDrag);

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setMouseTracking(true);
}

QPointF GraphicsView::mapImagePointToScene(qreal x, qreal y) const
{
	QGraphicsPixmapItem* pixmapItem = _view->getPixmapItem();
	return pixmapItem->mapToScene(x, y);
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
	setZoomValue(_zoomFactor);
}

void GraphicsView::showAnnotation(bool show)
{ 
	_showAnnotation = show;
	update();
}

void GraphicsView::zoomNormal()
{
	_zoomFactor = MAX_ZOOM / 2;
	setZoomValue(_zoomFactor);
}

void GraphicsView::zoom2x()
{
	_zoomFactor = MAX_ZOOM / 2 + ZOOM_STEP;
	setZoomValue(_zoomFactor);
}

void GraphicsView::zoom4x()
{
	_zoomFactor = MAX_ZOOM / 2 + ZOOM_STEP * 2;
	setZoomValue(_zoomFactor);
}

void GraphicsView::zoom8x()
{
	_zoomFactor = MAX_ZOOM / 2 + ZOOM_STEP * 3;
	setZoomValue(_zoomFactor);
}

void GraphicsView::zoomIn()
{
	_zoomFactor += ZOOM_STEP / 10;
	_zoomFactor = qMin(_zoomFactor, MAX_ZOOM);
	setZoomValue(_zoomFactor);
}

void GraphicsView::zoomOut()
{
	_zoomFactor -= ZOOM_STEP / 10;
	_zoomFactor = qMax(0, _zoomFactor);
	setZoomValue(_zoomFactor);
}

void GraphicsView::applyZoomValue()
{
	_zoomFactor = qMax(0, _zoomFactor);
	_zoomFactor = qMin(_zoomFactor, MAX_ZOOM);
	qreal scale = qPow(qreal(2), (_zoomFactor - MAX_ZOOM / 2) / qreal(ZOOM_STEP));

	QTransform transform;
	transform.scale(scale, scale);
	setTransform(transform);
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
		QPointF pointPixmap = pixmapItem->mapFromScene(pointScene);

		QImage image = pixmapItem->pixmap().toImage();
		if (pointPixmap.x() >= 0 && pointPixmap.x() < image.width() && pointPixmap.y() >= 0 && pointPixmap.y() < image.height())
		{
			float value = getGlobalImage()->getValue(QPoint(pointPixmap.x(), pointPixmap.y()));
			_strCoord = QString("X: %1, Y: %2, Val: %3").arg(QString::number(pointPixmap.x(), 'f', 0)).arg(QString::number(pointPixmap.y(), 'f', 0)).arg(QString::number(value, 'f', 1));


			QRgb rgbValue = image.pixel(QPoint(pointPixmap.x(), pointPixmap.y()));
			_strValue = QString("R:%1, G:%2, B:%3").arg(qRed(rgbValue), 3).arg(qGreen(rgbValue), 3).arg(qBlue(rgbValue), 3);
		}
		else
		{
			_strCoord.clear();
			_strValue.clear();
		}
		update();
	}

	MouseHandler::handleMove(event);

	QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	MouseHandler::handleRelease(event);

	QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::paintEvent(QPaintEvent* event)
{
	QGraphicsView::paintEvent(event);

	if (_showAnnotation)
	{
		QPainter painter(viewport());
		int fontHeight = rect().height() < 800 ? 12 : 16;
		QFont font("Arial", fontHeight);
		painter.setFont(font);
		painter.setPen(QPen(qRgb(255, 255, 150)));
	
		// Get the height of the font
		QFontMetrics fm(font);
		int pixelsHigh = fm.height() * 1.1;

		BaseImage* image = getGlobalImage();
		if (image)
		{
			QString str = QString(tr("Size: %1%2%3")).arg(image->width()).arg(QString(QChar(0x00D7))).arg(image->height());
			painter.drawText(QRect(0, 0, 200, pixelsHigh), Qt::AlignLeft, str);
		}

		QString str = QString(tr("WL: %1 WW: %2")).arg(QString::number(_view->windowLevel(), 'f', 1)).arg(QString::number(_view->windowWidth(), 'f', 1));
		painter.drawText(QRect(0, rect().bottom() - pixelsHigh, 400, pixelsHigh), Qt::AlignLeft, str);

		painter.setPen(QPen(qRgb(255, 100, 100)));
		painter.drawText(QRect(0, rect().bottom() - pixelsHigh * 2, 400, pixelsHigh), Qt::AlignLeft, _strValue);
		painter.drawText(QRect(0, rect().bottom() - pixelsHigh * 3, 400, pixelsHigh), Qt::AlignLeft, _strCoord);
	}
}
