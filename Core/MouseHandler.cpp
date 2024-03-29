#include "MouseHandler.h"

#include "GlobalFunc.h"
#include "View.h"
#include "Document.h"
#include "mainwindow.h"
#include "GraphicsView.h"
#include "ToolButton.h"
#include "../Image/BaseImage.h"

QPoint MouseHandler::_mousePos;
ToolButton* MouseHandler::_leftButton = nullptr;
ToolButton* MouseHandler::_rightButton = nullptr;

void MouseHandler::handlePress(QMouseEvent* event)
{
    _mousePos = event->pos();
    if (event->button() == Qt::LeftButton && _leftButton && _leftButton->isEnabled())
    {
        _leftButton->mouseHandler()->press(event);
    }
    else if(event->button() == Qt::RightButton && _rightButton && _rightButton->isEnabled())
    {
        _rightButton->mouseHandler()->press(event);
    }
}

void MouseHandler::handleMove(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton && _leftButton && _leftButton->isEnabled())
    {
        _leftButton->mouseHandler()->move(event);
    }
    else if (event->buttons() & Qt::RightButton && _rightButton && _rightButton->isEnabled())
    {
        _rightButton->mouseHandler()->move(event);
    }

    // No mouse button is pressed down
    if (!(event->buttons() & Qt::LeftButton) && !(event->buttons() & Qt::RightButton))
    {
        QPointF point = getGlobalActiveView()->view()->mapToScene(event->pos());
        getGlobalActiveView()->scene()->mouseMove(point);
    }
}

void MouseHandler::handleRelease(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && _leftButton && _leftButton->isEnabled())
    {
        _leftButton->mouseHandler()->release(event);
    }
    else if (event->button() == Qt::RightButton && _rightButton && _rightButton->isEnabled())
    {
        _rightButton->mouseHandler()->release(event);
    }
}

void MouseHandler::setLeftButton(ToolButton* button)
{
    _leftButton = button;
    if (_leftButton->mouseHandler())
    {
        _leftButton->mouseHandler()->active();
    }
}

void MouseHandler::unsetLeftButton()
{
    if (_leftButton)
    {
        if (_leftButton->mouseHandler())
        {
            _leftButton->mouseHandler()->deactive();
        }
        _leftButton = nullptr;
    }
}

void MouseHandler::setRightButton(ToolButton* button)
{
    _rightButton = button;
    if (_rightButton->mouseHandler())
    {
        _rightButton->mouseHandler()->active();
    }
}

void MouseHandler::unsetRightButton()
{
    if (_rightButton)
    {
        if (_rightButton->mouseHandler())
        {
            _rightButton->mouseHandler()->deactive();
        }
        _rightButton = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////

void SliceMouseHandler::press(QMouseEvent*)
{
    getGlobalActiveView()->view()->setDragMode(QGraphicsView::NoDrag);
}

void SliceMouseHandler::move(QMouseEvent* event)
{
    BaseImage* image = getGlobalImage();
    if (image == nullptr || image->slice() <= 1)
        return;

    QPoint offset = event->pos() - _mousePos;
    getGlobalActiveView()->slicePlus(offset.y());

    getGlobalDocument()->applyImageWidthAndLevel();

    _mousePos = event->pos();
}

//////////////////////////////////////////////////////////////////////////

void LocationMouseHandler::active()
{
    if (getGlobalImage()->slice() > 1)
    {
        getGlobalWindow()->showThreeViewAxes(true);
    }
}

void LocationMouseHandler::deactive()
{
    getGlobalWindow()->showThreeViewAxes(false);
}

void LocationMouseHandler::press(QMouseEvent* event)
{
    getGlobalActiveView()->view()->setDragMode(QGraphicsView::NoDrag);

    QPoint point = getGlobalActiveView()->view()->mapToScene(event->pos()).toPoint();
    getGlobalWindow()->setAxesLocation(point);
}

void LocationMouseHandler::move(QMouseEvent* event)
{
    QPoint point = getGlobalActiveView()->view()->mapToScene(event->pos()).toPoint();
    getGlobalWindow()->setAxesLocation(point);
}

//////////////////////////////////////////////////////////////////////////

void ImageWindowMouseHandler::move(QMouseEvent* event)
{
    if (getGlobalActiveView()->sceneMode() != MOVE_ITEM_TEMP)
    {
        // Calculate image window
        CalcImageWindow(_mousePos - event->pos());

        _mousePos = event->pos();
    }
}

void ImageWindowMouseHandler::release(QMouseEvent*)
{
    _horzOrVert = 0;
}

void ImageWindowMouseHandler::CalcImageWindow(QPoint offset)
{
    BaseImage* image = getGlobalImage();
    if (image == nullptr)
        return;

    float windowWidth = getGlobalActiveView()->windowWidth();
    float windowLevel = getGlobalActiveView()->windowLevel();

    float minValue = image->getMinValue();
    float maxValue = image->getMaxValue();

    QRect rect = getGlobalActiveView()->rect();
    if (abs(offset.x()) > abs(offset.y()) * 1.5f && _horzOrVert != -1)
    {
        // Modify window width
        _horzOrVert = 1;

        float fRatio = float(offset.x()) / rect.width() * 2;
        // Define the minimum value of window width
        float threshold = (maxValue - minValue) * 0.01f;

        float tempWidth = windowWidth - (maxValue - minValue) * fRatio;
        windowWidth = (tempWidth > threshold) ? tempWidth : threshold;
    }
    else if(abs(offset.y()) > abs(offset.x()) * 1.5f && _horzOrVert != 1)
    {
        // Modify window level
        _horzOrVert = -1;

        float fRatio = float(offset.y()) / rect.height() * 2;
        windowLevel += (maxValue - minValue) * fRatio;
    }

    if (getGlobalWindow()->isViewLinked())
    {
        getGlobalAxialView()->setWindowWidthAndLevel(windowWidth, windowLevel);
        getGlobalCoronalView()->setWindowWidthAndLevel(windowWidth, windowLevel);
        getGlobalSagittalView()->setWindowWidthAndLevel(windowWidth, windowLevel);
    }
    else
    {
        getGlobalActiveView()->setWindowWidthAndLevel(windowWidth, windowLevel);
    }

    getGlobalDocument()->applyImageWidthAndLevel();
}

//////////////////////////////////////////////////////////////////////////

void ROIWindowMouseHandler::deactive()
{
    getGlobalActiveView()->view()->setDragMode(QGraphicsView::NoDrag);
}

void ROIWindowMouseHandler::press(QMouseEvent*)
{
    getGlobalActiveView()->view()->setDragMode(QGraphicsView::RubberBandDrag);
}

void ROIWindowMouseHandler::release(QMouseEvent* event)
{
    View* view = getGlobalActiveView();
    view->ROIWindow(QRectF(view->view()->mapToScene(_mousePos), view->view()->mapToScene(event->pos())));
}

//////////////////////////////////////////////////////////////////////////

void ZoomMouseHandler::press(QMouseEvent*)
{
    getGlobalActiveView()->view()->setDragMode(QGraphicsView::NoDrag);
}

void ZoomMouseHandler::move(QMouseEvent* event)
{
    QPoint delta = _mousePos - event->pos();
    getGlobalWindow()->setZoomValueOffset(delta.y());
    _mousePos = event->pos();
}

//////////////////////////////////////////////////////////////////////////

void MagnifierMouseHandler::press(QMouseEvent*)
{
    getGlobalActiveView()->view()->setDragMode(QGraphicsView::NoDrag);
    getGlobalActiveView()->view()->setCursor(Qt::CrossCursor);
    getGlobalActiveView()->view()->showMagnifier();
}

//////////////////////////////////////////////////////////////////////////

void SelectMouseHandler::deactive()
{
    getGlobalActiveView()->view()->setDragMode(QGraphicsView::NoDrag);
}

void SelectMouseHandler::press(QMouseEvent*)
{
    getGlobalActiveView()->setSceneMode(MOVE_ITEM);
}

void SelectMouseHandler::move(QMouseEvent*)
{
    getGlobalActiveView()->scene()->update();
}

//////////////////////////////////////////////////////////////////////////

void MoveMouseHandler::deactive()
{
    getGlobalActiveView()->view()->setDragMode(QGraphicsView::NoDrag);
}

void MoveMouseHandler::press(QMouseEvent*)
{
    getGlobalActiveView()->setSceneMode(MOVE_SCENE);
}

void MoveMouseHandler::move(QMouseEvent* event)
{
    if (getGlobalActiveView()->sceneMode() == MOVE_SCENE)
    {
        QPointF delta = getGlobalActiveView()->view()->mapToScene(_mousePos) - getGlobalActiveView()->view()->mapToScene(event->pos());
        _mousePos = event->pos();

        QRectF rect = getGlobalActiveView()->view()->sceneRect();
        getGlobalActiveView()->view()->setSceneRect(rect.x() + delta.x(), rect.y() + delta.y(), rect.width(), rect.height());
    }
}

//////////////////////////////////////////////////////////////////////////

void DrawMouseHandler::press(QMouseEvent* event)
{
    if (getGlobalActiveView()->sceneMode() != MOVE_ITEM_TEMP)
    {
        getGlobalActiveView()->setSceneMode(INSERT_ITEM);
    }
    QPointF point = getGlobalActiveView()->view()->mapToScene(event->pos());
    getGlobalActiveView()->scene()->mousePress(point);
}

void DrawMouseHandler::move(QMouseEvent* event)
{
    QPointF point = getGlobalActiveView()->view()->mapToScene(event->pos());
    getGlobalActiveView()->scene()->mouseMove(point);
}

void DrawMouseHandler::release(QMouseEvent* event)
{
    QPointF point = getGlobalActiveView()->view()->mapToScene(event->pos());
    getGlobalActiveView()->scene()->mouseRelease(point);
}
