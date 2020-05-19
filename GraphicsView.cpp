#include "GraphicsView.h"

#include <QtMath>
#include <QDebug>
#include "View.h"

GraphicsView::GraphicsView(View* view, QGraphicsScene* scene, QWidget* parent)
	: QGraphicsView(scene, parent)
	, _view(view)
	, _value(MAX_ZOOM / 2)
{
	setDragMode(QGraphicsView::NoDrag);//ScrollHandDrag
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
	_value = value;
	applyValue();
}

void GraphicsView::zoomIn()
{
	_value += 5;
	_value = qMin(_value, MAX_ZOOM);
	emit valueChanged(_value);
}

void GraphicsView::zoomOut()
{
	_value -= 5;
	_value = qMax(0, _value);
	emit valueChanged(_value);
}

void GraphicsView::zoomNormal()
{
	_value = MAX_ZOOM / 2;
	emit valueChanged(_value);
}

void GraphicsView::applyValue()
{
	_value = qMax(0, _value);
	_value = qMin(_value, MAX_ZOOM);
	qreal scale = qPow(qreal(2), (_value - MAX_ZOOM / 2) / qreal(50));

	QMatrix matrix;
	matrix.scale(scale, scale);
	setMatrix(matrix);
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	QPoint point = event->pos();

	QPointF pointScene = mapToScene(point);

	QGraphicsPixmapItem* pixmapItem = _view->getPixmapItem();
	QPointF pointPixemap = pixmapItem->mapFromScene(pointScene);

	QString strCoord = QString("x:%1, y:%2").arg(QString::number(pointPixemap.x(), 'f', 0)).arg(QString::number(pointPixemap.y(), 'f', 0));
	emit showCoordinate(strCoord);

	QString strValue;
	QImage image = pixmapItem->pixmap().toImage();
	if (pointPixemap.x() >=0 && pointPixemap.x() < image.width() && pointPixemap.y() >= 0 && pointPixemap.y() < image.height())
	{
		QRgb value = image.pixel(QPoint(pointPixemap.x(), pointPixemap.y()));
		strValue = QString("R:%1, G:%2, B:%3").arg(qRed(value), 3).arg(qGreen(value), 3).arg(qBlue(value), 3);
	}
	emit showPixelValue(strValue);

	QGraphicsView::mouseMoveEvent(event);
}