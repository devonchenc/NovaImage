#include "GraphicsView.h"

#include <QtMath>
#include <QMimeData>
#include <QDebug>

#include "View.h"
#include "MouseHandler.h"
#include "GlobalFunc.h"
#include "mainwindow.h"
#include "../Image/BaseImage.h"
#include "../Widget/MagnifierWidget.h"

GraphicsView::GraphicsView(View* view, QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent)
    , _view(view)
    , _zoomFactor(MAX_ZOOM / 2)
    , _showAnnotation(true)
    , _showCrossLine(false)
    , _showLineScale(true)
    , _magnifier(new MagnifierWidget(this))
{
    setDragMode(QGraphicsView::NoDrag);

    _magnifier->installEventFilter(this);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setMouseTracking(true);
}

QPointF GraphicsView::mapImagePointToScene(qreal x, qreal y) const
{
    QGraphicsPixmapItem* pixmapItem = _view->getPixmapItem();
    return pixmapItem->mapToScene(x, y);
}

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

void GraphicsView::showCrossLine(bool show)
{
    _showCrossLine = show;
    update();
}

void GraphicsView::showLineScale(bool show)
{
    _showLineScale = show;
    update();
}

void GraphicsView::showMagnifier()
{
    _magnifier->show();
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
    qreal ratio = qPow(qreal(2), (_zoomFactor - MAX_ZOOM / 2) / qreal(ZOOM_STEP));

    QTransform transform;
    transform.scale(ratio, ratio);
    setTransform(transform);
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
    getGlobalWindow()->setActiveView(_view);

    MouseHandler::handlePress(event);

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    // Not directly use event->pos
    QPoint mousePoint = mapFromGlobal(event->globalPos()) - QPoint(1, 1);
    QGraphicsPixmapItem* pixmapItem = _view->getPixmapItem();
    if (pixmapItem)
    {
        QPointF pointScene = mapToScene(mousePoint);
        QPointF pointPixmap = pixmapItem->mapFromScene(pointScene);

        QImage image = pixmapItem->pixmap().toImage();
        if (pointPixmap.x() >= 0 && pointPixmap.x() < image.width() && pointPixmap.y() >= 0 && pointPixmap.y() < image.height())
        {
            float value = _view->getImageValue(pointPixmap.x(), pointPixmap.y());
            int decimal = 4;
            if (abs(value) > 10.0f)
            {
                decimal = 1;
            }
            else if (abs(value) > 1.0f)
            {
                decimal = 2;
            }
            else if (abs(value) > 0.1f)
            {
                decimal = 3;
            }
            _strCoord = QString(tr("X: %1, Y: %2, Val: %3")).arg(QString::number(pointPixmap.x(), 'f', 0)).arg(QString::number(pointPixmap.y(), 'f', 0)).arg(QString::number(value, 'f', decimal));

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

void GraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    getGlobalWindow()->singleView();

    QGraphicsView::mouseDoubleClickEvent(event);
}

void GraphicsView::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        if (event->angleDelta().y() > 0)
            zoomIn();
        else
            zoomOut();

        event->accept();
    }
    else
    {
        BaseImage* image = getGlobalImage();
        if (image)
        {
            if (event->angleDelta().y() > 0)
                _view->slicePlusOne();
            else
                _view->sliceMinusOne();
        }

    //    QGraphicsView::wheelEvent(event);
    }
}

void GraphicsView::dragEnterEvent(QDragEnterEvent* event)
{
    // Ignore drag event to make mainwindow handle it
    event->ignore();
}

void GraphicsView::drawForeground(QPainter*, const QRectF&)
{
    BaseImage* image = getGlobalImage();
    if (image == nullptr)
        return;

    if (_showAnnotation)
    {
        drawAnnotation();
    }
    if (_showCrossLine)
    {
        drawCrossLine();
    }
    if (_showLineScale)
    {
        drawLineScale();
    }
}

bool GraphicsView::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == _magnifier && event->type() == QEvent::MouseMove)
    {
        // In order to track the image pixel when magnifying
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        mouseMoveEvent(mouseEvent);
        return false;
    }
    else
    {
        return QGraphicsView::eventFilter(obj, event);
    }
}

void GraphicsView::focusInEvent(QFocusEvent*)
{
    //getGlobalWindow()->setActiveView(_view);
    _view->update();
}

void GraphicsView::focusOutEvent(QFocusEvent*)
{
    //getGlobalWindow()->deactiveView(_view);
    _view->update();
}

void GraphicsView::drawAnnotation()
{
    int fontHeight = 16;
    if (rect().height() < 400)
    {
        fontHeight = 8;
    }
    else if (rect().height() < 600)
    {
        fontHeight = 10;
    }
    else if (rect().height() < 800)
    {
        fontHeight = 12;
    }

    QFont font("Arial", fontHeight);
    QPainter painter(viewport());
    painter.setFont(font);
    painter.setPen(QPen(qRgb(255, 255, 150)));

    // Get the height of the font
    int pixelsHigh = painter.fontMetrics().height() * 1.1;
    int y = rect().bottom() - 10;

    qreal scale = qPow(qreal(2), (_zoomFactor - MAX_ZOOM / 2) / qreal(ZOOM_STEP));
    QString str = QString(tr("Zoom: %1%")).arg(QString::number(scale * 100.0, 'f', 2));
    painter.drawText(QRect(0, 0, 240, pixelsHigh), Qt::AlignLeft, str);

    str = QString(tr("Size: %1%2%3")).arg(_view->imageWidth()).arg(QString(QChar(0x00D7))).arg(_view->imageHeight());
    painter.drawText(QRect(0, pixelsHigh, 350, pixelsHigh), Qt::AlignLeft, str);

    str = QString(tr("Slice: %1/%2")).arg(_view->imageCurrentSlice() + 1).arg(_view->imageSlice());
    painter.drawText(QRect(0, pixelsHigh * 2, 350, pixelsHigh), Qt::AlignLeft, str);

    float windowLevel = _view->windowLevel();
    float windowWidth = _view->windowWidth();
    int decimal = 4;
    if (abs(windowLevel) > 10.0f)
    {
        decimal = 1;
    }
    else if (abs(windowLevel) > 1.0f)
    {
        decimal = 2;
    }
    else if (abs(windowLevel) > 0.1f)
    {
        decimal = 3;
    }
    str = QString(tr("WL: %1 WW: %2")).arg(QString::number(windowLevel, 'f', decimal)).arg(QString::number(windowWidth, 'f', decimal));
    painter.drawText(QRect(0, y - pixelsHigh, 400, pixelsHigh), Qt::AlignLeft, str);

    painter.setPen(QPen(qRgb(255, 100, 100)));
    painter.drawText(QRect(0, y - pixelsHigh * 2, 600, pixelsHigh), Qt::AlignLeft, _strValue);
    painter.drawText(QRect(0, y - pixelsHigh * 3, 600, pixelsHigh), Qt::AlignLeft, _strCoord);
}

void GraphicsView::drawCrossLine()
{
    QRectF rect = _view->getPixmapItem()->sceneBoundingRect();
    QPoint topLeft = mapFromScene(rect.topLeft());
    QPoint center = mapFromScene(rect.center());
    QPoint bottomRight = mapFromScene(rect.bottomRight());
    QPainter painter(viewport());
    painter.setPen(QPen(qRgb(255, 50, 50)));
    painter.drawLine(topLeft.x(), center.y(), bottomRight.x(), center.y());
    painter.drawLine(center.x(), topLeft.y(), center.x(), bottomRight.y());
}

void GraphicsView::drawLineScale()
{
    BaseImage* image = getGlobalImage();
    if (!image->hasPixelSpacing())
        return;

    float horzPixelSpacing = image->horzPixelSpacing();
    float vertPixelSpacing = image->vertPixelSpacing();

    float horzDistance = calcScale(horzPixelSpacing);
    float vertDistance = calcScale(vertPixelSpacing);

    qreal scale = qPow(qreal(2), (_zoomFactor - MAX_ZOOM / 2) / qreal(ZOOM_STEP));
    float horzInterval = horzDistance * scale / horzPixelSpacing;
    float vertInterval = vertDistance * scale / vertPixelSpacing;
    int margin = 5;
    int x = rect().right() - margin;
    int y = rect().bottom() - margin;

    QPainter painter(viewport());
    painter.setPen(QPen(qRgb(255, 255, 100)));
    painter.drawLine(rect().center().x() - 5 * horzInterval, y, rect().center().x() + 5 * horzInterval, y);
    painter.drawLine(x, rect().center().y() - 5 * vertInterval, x, rect().center().y() + 5 * vertInterval);

    painter.setPen(QPen(qRgb(255, 255, 0)));
    for (int i = 0; i <= 10; i++)
    {
        painter.drawLine(rect().center().x() + (i - 5) * horzInterval, y, rect().center().x() + (i - 5) * horzInterval, y - 5);
        if (i % 5 == 0)
        {
            painter.drawLine(rect().center().x() + (i - 5) * horzInterval, y, rect().center().x() + (i - 5) * horzInterval, y - 10);
        }
        painter.drawLine(x, rect().center().y() + (i - 5) * vertInterval, x - 5, rect().center().y() + (i - 5) * vertInterval);
        if (i % 5 == 0)
        {
            painter.drawLine(x, rect().center().y() + (i - 5) * vertInterval, x - 10, rect().center().y() + (i - 5) * vertInterval);
        }
    }

    QString str = QString::number(horzDistance, 'f', 2) + " mm";
    painter.drawText(QRect(rect().center().x(), y - 23, 9 * horzInterval, 20), Qt::AlignCenter, str);

    painter.rotate(-90);
    str = QString::number(vertDistance, 'f', 2) + " mm";
    painter.drawText(QRect(-rect().center().y(), rect().width() - 23 - margin, 9 * horzInterval, 20), Qt::AlignCenter, str);
}

float GraphicsView::calcScale(float pixelSpacing)
{
    float distance = 20.0f;
    int flag = 0;
    while (distance / pixelSpacing > 60)
    {
        if (flag == 0)
        {
            distance /= 2;
        }
        else if (flag == 1)
        {
            distance /= 2;
        }
        else if (flag == 2)
        {
            distance /= 2.5;
        }
        flag = (flag + 1) % 3;
    }

    return distance;
}
