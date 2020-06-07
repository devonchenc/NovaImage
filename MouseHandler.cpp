#include "MouseHandler.h"

#include <QDebug>

#include "GlobalFunc.h"
#include "View.h"
#include "GraphicsView.h"
#include "ToolButton.h"
#include "Image/BaseImage.h"
#include "Widget/LevelsProcessor.h"

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

ImageWindowMouseHandler::ImageWindowMouseHandler()
{
	_bottom = getGlobalImage()->getMinValue();
	_top = getGlobalImage()->getMaxValue();
}

void ImageWindowMouseHandler::press(QMouseEvent* event)
{
}

void ImageWindowMouseHandler::move(QMouseEvent* event)
{
	QPoint delta = _mousePos - event->pos();
	_mousePos = event->pos();

	// Calculate image window
	float bottom, top;
	if (CalcImageWindow(delta, bottom, top))
	{
		LevelsProcessor processor;
		processor.setPara(bottom, 1.0f, top);
		processor.process(getGlobalImage());
	}

	repaintView();
}

void ImageWindowMouseHandler::release(QMouseEvent* event)
{
	_horzOrVert = 0;
}

bool ImageWindowMouseHandler::CalcImageWindow(QPoint point, float& bottom, float& top)
{
	BaseImage* image = getGlobalImage();
	float minValue = image->getMinValue();
	float maxValue = image->getMaxValue();

	QRect rect = getGlobalView()->rect();
	if (abs(point.x()) > abs(point.y()) * 1.5f && _horzOrVert != -1)			// 鼠标横向平移
	{
		// 缩小窗
		_horzOrVert = 1;

		float fRatio = float(point.x()) / rect.width() * 0.5f;

		float tempBottom = _bottom + (maxValue - minValue) * fRatio;
		float tempTop = _top - (maxValue - minValue) * fRatio;
		// Define the minimum gap between bottom and top
		float threshold = (maxValue - minValue) * 0.01f;
		if (tempBottom < tempTop - threshold)
		{
			bottom = tempBottom;
			top = tempTop;
		}
		else
		{
			bottom = (tempTop + tempBottom) / 2.0f - threshold / 2.0f;
			top = (tempTop + tempBottom) / 2.0f + threshold / 2.0f;
		}

		_bottom = bottom;
		_top = top;
		return true;
	}
	else if(abs(point.y()) > abs(point.x()) * 1.5f && _horzOrVert != 1)	// 鼠标纵向平移
	{
		// 平移窗
		_horzOrVert = -1;

		float fRatio = float(point.y()) / rect.height() * 0.5f;
		qDebug() << fRatio << (maxValue - minValue) * fRatio << bottom << top;

		_bottom += (maxValue - minValue) * fRatio;
		_top += (maxValue - minValue) * fRatio * 0.5f;

		bottom = _bottom;
		top = _top;

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

void ZoomMouseHandler::press(QMouseEvent* event)
{

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

void SelectMouseHandler::press(QMouseEvent* event)
{

}

void SelectMouseHandler::move(QMouseEvent* event)
{
	getGlobalView()->scene()->update();
}

void SelectMouseHandler::release(QMouseEvent* event)
{

}

//////////////////////////////////////////////////////////////////////////

void MoveMouseHandler::press(QMouseEvent* event)
{

}

void MoveMouseHandler::move(QMouseEvent* event)
{
	QPointF delta = getGlobalView()->view()->mapToScene(_mousePos) - getGlobalView()->view()->mapToScene(event->pos());
	_mousePos = event->pos();

	QRectF rect = getGlobalView()->view()->sceneRect();
	getGlobalView()->view()->setSceneRect(rect.x() + delta.x(), rect.y() + delta.y(), rect.width(), rect.height());
}

void MoveMouseHandler::release(QMouseEvent* event)
{

}

//////////////////////////////////////////////////////////////////////////

void DrawMouseHandler::press(QMouseEvent* event)
{
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