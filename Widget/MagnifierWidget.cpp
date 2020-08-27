#include "MagnifierWidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QCursor>
#include <QWindow>
#include <QScreen>
#include <QTimer>
#include <QMouseEvent>

const int grabInterval = 50;
const int sizeOfMouseIcon = 20;

MagnifierWidget::MagnifierWidget(QWidget* parent)
    : QWidget(parent)
    , _displayDirection(TopLeft)
    , _magnifyAeraSize(41, 41)
    , _magnifyTimes(8)
    , _magnifierSize((_magnifyAeraSize - QSize(2, 2)) * _magnifyTimes)
    , _totalSize(_magnifyAeraSize + _magnifierSize)
    , _timer(new QTimer(this))
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(_totalSize);
    setMouseTracking(true);
    _timer->setInterval(grabInterval);
    connect(_timer, &QTimer::timeout, this, &MagnifierWidget::updatePosition);
    _timer->start();
}

MagnifierWidget::~MagnifierWidget()
{
    delete _timer;
}

void MagnifierWidget::setMagnifyArea(QSize size)
{
    _magnifyAeraSize = size;
    update();
}

void MagnifierWidget::setMagnifyTimes(int times)
{
    _magnifyTimes = times;
    update();
}

void MagnifierWidget::paintEvent(QPaintEvent*)
{
    QWindow* window = QApplication::desktop()->windowHandle();
    QPixmap grab = window->screen()->grabWindow(QApplication::desktop()->winId());
    grab = grab.copy(QCursor::pos().x() - _magnifyAeraSize.width() / 2 + 1, QCursor::pos().y() - _magnifyAeraSize.height() / 2 + 1,
                     _magnifyAeraSize.width() - 2, _magnifyAeraSize.height() - 2);

    QPainter painter(this);
    QPixmap centerPixel = grab.copy(_magnifyAeraSize.width() / 2 - 1, _magnifyAeraSize.height() / 2 - 1, 1, 1);
    QColor color = centerPixel.toImage().pixel(0, 0);
    if (color.red() > 100 && color.blue() > 100 && color.green() > 100)
        painter.setPen(QColor(0, 0, 0));
    else
        painter.setPen(QColor(255, 255, 255));

    QPoint magnifyAeraPoint, magnifierPoint;
    if (_displayDirection == TopLeft)
    {
        magnifyAeraPoint = QPoint(0, 0);
        magnifierPoint = QPoint(_magnifyAeraSize.width(), _magnifyAeraSize.height());
    }
    else if(_displayDirection == TopRight)
    {
        magnifyAeraPoint = QPoint(_magnifierSize.width(), 0);
        magnifierPoint = QPoint(0, _magnifyAeraSize.height());
    }
    else if (_displayDirection == BottomLeft)
    {
        magnifyAeraPoint = QPoint(0, _magnifierSize.height());
        magnifierPoint = QPoint(_magnifyAeraSize.width(), 0);
    }
    else if (_displayDirection == BottomRight)
    {
        magnifyAeraPoint = QPoint(_magnifierSize.width(), _magnifierSize.height());
        magnifierPoint = QPoint(0, 0);
    }
    painter.drawRect(magnifyAeraPoint.x(), magnifyAeraPoint.y(), _magnifyAeraSize.width() - 1, _magnifyAeraSize.height() - 1);
    painter.drawRect(magnifierPoint.x(), magnifierPoint.y(), _magnifierSize.width() - 1, _magnifierSize.height() - 1);
    painter.drawPixmap(magnifierPoint.x(), magnifierPoint.y(), _magnifierSize.width(), _magnifierSize.height(), grab);

    static int change = 0;
    change++;
    if (change > 3)
    {
        painter.setPen(QColor(0, 0, 0));
        if (change >= 6)
            change = 0;
    }
    else
    {
        painter.setPen(QColor(255, 255, 255));
    }
    QPoint leftop(magnifierPoint.x() + _magnifierSize.width() / 2 - _magnifyTimes / 2, magnifierPoint.y() + _magnifierSize.height() / 2 - _magnifyTimes / 2);
    painter.drawRect(leftop.x(), leftop.y(), _magnifyTimes, _magnifyTimes);
}

void MagnifierWidget::mousePressEvent(QMouseEvent*)
{
    if (parentWidget())
    {
        parentWidget()->setCursor(Qt::ArrowCursor);
    }
    close();
}

void MagnifierWidget::mouseReleaseEvent(QMouseEvent*)
{
    if (parentWidget())
    {
        parentWidget()->setCursor(Qt::ArrowCursor);
    }
    close();
}

void MagnifierWidget::updatePosition()
{
    QPoint position = QCursor::pos() - QPoint(_magnifyAeraSize.width() / 2, _magnifyAeraSize.height() / 2);
    bool right = position.x() + _totalSize.width() > QApplication::desktop()->width() ? true : false;
    bool bottom = position.y() + _totalSize.height() > QApplication::desktop()->height() ? true : false;
    if (right && bottom)
    {
        _displayDirection = BottomRight;
        position.setX(position.x() - _magnifierSize.width());
        position.setY(position.y() - _magnifierSize.height());
    }
    else if (right)
    {
        _displayDirection = TopRight;
        position.setX(position.x() - _magnifierSize.width());
    }
    else if (bottom)
    {
        _displayDirection = BottomLeft;
        position.setY(position.y() - _magnifierSize.height());
    }
    else
    {
        _displayDirection = TopLeft;
    }

    move(position);
    update();
}
