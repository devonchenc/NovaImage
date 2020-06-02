#include "MouseHandler.h"

#include "GlobalFunc.h"
#include "View.h"
#include "GraphicsView.h"

QPoint MouseHandler::_mousePos;
MouseHandler* MouseHandler::_leftHandler = nullptr;
MouseHandler* MouseHandler::_rightHandler = nullptr;

void MouseHandler::handlePress(QMouseEvent* event)
{
	_mousePos = event->pos();
	if (event->button() == Qt::LeftButton && _leftHandler)
	{
		_leftHandler->press(event);
	}
	else if(event->button() == Qt::RightButton && _rightHandler)
	{
		_rightHandler->press(event);
	}
}

void MouseHandler::handleMove(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton && _leftHandler)
	{
		_leftHandler->move(event);
	}
	else if (event->buttons() & Qt::RightButton && _rightHandler)
	{
		_rightHandler->move(event);
	}
}

void MouseHandler::handleRelease(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && _leftHandler)
	{
		_leftHandler->release(event);
	}
	else if (event->button() == Qt::RightButton && _rightHandler)
	{
		_rightHandler->release(event);
	}
}

void MouseHandler::setLeftHandler(MouseHandler* handler)
{
	_leftHandler = handler;
}

void MouseHandler::setRightHandler(MouseHandler* handler)
{
	_rightHandler = handler;
}

//////////////////////////////////////////////////////////////////////////

void ZoomMouseHandler::press(QMouseEvent* event)
{

}

void ZoomMouseHandler::move(QMouseEvent* event)
{

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

}

void SelectMouseHandler::release(QMouseEvent* event)
{

}

//////////////////////////////////////////////////////////////////////////

void MoveMouseHandler::press(QMouseEvent* event)
{
	int a = 5;
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