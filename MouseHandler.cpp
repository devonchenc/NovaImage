#include "MouseHandler.h"


void MouseHandler::handlePress(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{

	}
	else if(event->button() == Qt::RightButton)
	{

	}
}

void MouseHandler::handleMove(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{

	}
	else if (event->button() == Qt::RightButton)
	{

	}
}

void MouseHandler::handleRelease(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{

	}
	else if (event->button() == Qt::RightButton)
	{

	}
}


void ZoomMouseHandler::press(QMouseEvent* event)
{

}

void ZoomMouseHandler::move(QMouseEvent* event)
{

}

void ZoomMouseHandler::release(QMouseEvent* event)
{

}