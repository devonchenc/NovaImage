#include "MouseHandler.h"

#include <QDebug>

#include "GlobalFunc.h"
#include "View.h"
#include "Document.h"
#include "GraphicsView.h"
#include "ToolButton.h"
#include "../Image/BaseImage.h"

QPoint MouseHandler::_mousePos;
ToolButton* MouseHandler::_leftButton = nullptr;
ToolButton* MouseHandler::_rightButton = nullptr;

void MouseHandler::handlePress(QMouseEvent* event)
{
	_mousePos = event->pos();
	if (event->button() == Qt::LeftButton && _leftButton)
	{
		_leftButton->mouseHandler()->press(event);
	}
	else if(event->button() == Qt::RightButton && _rightButton)
	{
		_rightButton->mouseHandler()->press(event);
	}
}

void MouseHandler::handleMove(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton && _leftButton)
	{
		_leftButton->mouseHandler()->move(event);
	}
	else if (event->buttons() & Qt::RightButton && _rightButton)
	{
		_rightButton->mouseHandler()->move(event);
	}

	// No mouse button is pressed down
	if (!(event->buttons() & Qt::LeftButton) && !(event->buttons() & Qt::RightButton))
	{
		QPointF point = getGlobalView()->view()->mapToScene(event->pos());
		getGlobalView()->scene()->mouseMove(point);
	}
}

void MouseHandler::handleRelease(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && _leftButton)
	{
		_leftButton->mouseHandler()->release(event);
	}
	else if (event->button() == Qt::RightButton && _rightButton)
	{
		_rightButton->mouseHandler()->release(event);
	}
}

void MouseHandler::setLeftButton(ToolButton* button)
{
	_leftButton = button;
}

void MouseHandler::setRightButton(ToolButton* button)
{
	_rightButton = button;
}
//////////////////////////////////////////////////////////////////////////

void ImageWindowMouseHandler::press(QMouseEvent* event)
{

}

void ImageWindowMouseHandler::move(QMouseEvent* event)
{
	if (getGlobalView()->sceneMode() != MOVE_ITEM_TEMP)
	{
		QPoint delta = _mousePos - event->pos();
		_mousePos = event->pos();

		// Calculate image window
		CalcImageWindow(delta);

		repaintView();
	}
}

void ImageWindowMouseHandler::release(QMouseEvent* event)
{
	_horzOrVert = 0;
}

void ImageWindowMouseHandler::CalcImageWindow(QPoint point)
{
	BaseImage* image = getGlobalImage();
	if (image == nullptr)
		return;

	float windowWidth = getGlobalView()->windowWidth();
	float windowLevel = getGlobalView()->windowLevel();

	float minValue = image->getMinValue();
	float maxValue = image->getMaxValue();

	QRect rect = getGlobalView()->rect();
	if (abs(point.x()) > abs(point.y()) * 1.5f && _horzOrVert != -1)
	{
		// Modify window width
		_horzOrVert = 1;

		float fRatio = float(point.x()) / rect.width() * 2;
		// Define the minimum value of window width
		float threshold = (maxValue - minValue) * 0.01f;

		float tempWidth = windowWidth - (maxValue - minValue) * fRatio;
		windowWidth = (tempWidth > threshold) ? tempWidth : threshold;
	}
	else if(abs(point.y()) > abs(point.x()) * 1.5f && _horzOrVert != 1)
	{
		// Modify window level
		_horzOrVert = -1;

		float fRatio = float(point.y()) / rect.height() * 2;
		windowLevel += (maxValue - minValue) * fRatio;
	}

	getGlobalView()->setWindowWidthAndLevel(windowWidth, windowLevel);
}

//////////////////////////////////////////////////////////////////////////

void ROIWindowMouseHandler::unbounded()
{
	getGlobalView()->view()->setDragMode(QGraphicsView::NoDrag);
}

void ROIWindowMouseHandler::press(QMouseEvent* event)
{
	getGlobalView()->view()->setDragMode(QGraphicsView::RubberBandDrag);
}

void ROIWindowMouseHandler::move(QMouseEvent* event)
{

}

void ROIWindowMouseHandler::release(QMouseEvent* event)
{
	getGlobalDocument()->ROIWindow(QRectF(getGlobalView()->view()->mapToScene(_mousePos),
		getGlobalView()->view()->mapToScene(event->pos())));
}

//////////////////////////////////////////////////////////////////////////

void ZoomMouseHandler::press(QMouseEvent* event)
{
	getGlobalView()->view()->setDragMode(QGraphicsView::NoDrag);
}

void ZoomMouseHandler::move(QMouseEvent* event)
{
	QPoint delta = _mousePos - event->pos();
	_mousePos = event->pos();

	getGlobalView()->view()->setZoomValueOffset(delta.y());
}

void ZoomMouseHandler::release(QMouseEvent* event)
{

}

//////////////////////////////////////////////////////////////////////////

void MagnifierMouseHandler::press(QMouseEvent* event)
{
	getGlobalView()->view()->setCursor(Qt::CrossCursor);
	getGlobalView()->view()->showMagnifier();
}

void MagnifierMouseHandler::move(QMouseEvent* event)
{

}

void MagnifierMouseHandler::release(QMouseEvent* event)
{

}

//////////////////////////////////////////////////////////////////////////

void SelectMouseHandler::unbounded()
{
	getGlobalView()->view()->setDragMode(QGraphicsView::NoDrag);
}

void SelectMouseHandler::press(QMouseEvent* event)
{
	getGlobalView()->setSceneMode(MOVE_ITEM);
}

void SelectMouseHandler::move(QMouseEvent* event)
{
	getGlobalView()->scene()->update();
}

void SelectMouseHandler::release(QMouseEvent* event)
{

}

//////////////////////////////////////////////////////////////////////////

void MoveMouseHandler::unbounded()
{
	getGlobalView()->view()->setDragMode(QGraphicsView::NoDrag);
}

void MoveMouseHandler::press(QMouseEvent* event)
{
	getGlobalView()->setSceneMode(MOVE_SCENE);
}

void MoveMouseHandler::move(QMouseEvent* event)
{
	if (getGlobalView()->sceneMode() == MOVE_SCENE)
	{
		QPointF delta = getGlobalView()->view()->mapToScene(_mousePos) - getGlobalView()->view()->mapToScene(event->pos());
		_mousePos = event->pos();

		QRectF rect = getGlobalView()->view()->sceneRect();
		getGlobalView()->view()->setSceneRect(rect.x() + delta.x(), rect.y() + delta.y(), rect.width(), rect.height());
	}
}

void MoveMouseHandler::release(QMouseEvent* event)
{

}

//////////////////////////////////////////////////////////////////////////

void DrawMouseHandler::press(QMouseEvent* event)
{
	if (getGlobalView()->sceneMode() != MOVE_ITEM_TEMP)
	{
		getGlobalView()->setSceneMode(INSERT_ITEM);
	}
	QPointF point = getGlobalView()->view()->mapToScene(event->pos());
	getGlobalView()->scene()->mousePress(point);
}

void DrawMouseHandler::move(QMouseEvent* event)
{
	QPointF point = getGlobalView()->view()->mapToScene(event->pos());
	getGlobalView()->scene()->mouseMove(point);
}

void DrawMouseHandler::release(QMouseEvent* event)
{
	QPointF point = getGlobalView()->view()->mapToScene(event->pos());
	getGlobalView()->scene()->mouseRelease(point);
}