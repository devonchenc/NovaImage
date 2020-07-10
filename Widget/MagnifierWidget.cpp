#include "MagnifierWidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QCursor>
#include <QWindow>
#include <QScreen>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>

const int grabInterval = 50;
const int sizeOfMouseIcon = 20;

MagnifierWidget::MagnifierWidget(QWidget* parent)
	: QWidget(parent)
	, _size(190, 190)
	, _magnificationTimes(10)
	, _timer(new QTimer)
{
	setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setFixedSize(_size);
	setMouseTracking(true);
	_timer->setInterval(grabInterval);
	connect(_timer, &QTimer::timeout, this, &MagnifierWidget::updatePosition);
	_timer->start();
}

MagnifierWidget::~MagnifierWidget()
{
	delete _timer;
}

void MagnifierWidget::paintEvent(QPaintEvent*)
{
	QWindow* window = QApplication::desktop()->windowHandle();
	QPixmap grab = window->screen()->grabWindow(QApplication::desktop()->winId());
	grab = grab.copy(QCursor::pos().x() - _size.width() / _magnificationTimes / 2, QCursor::pos().y() - _size.height() / _magnificationTimes / 2,
		_size.width() / _magnificationTimes, _size.height() / _magnificationTimes);

	QPainter painter(this);
	painter.drawPixmap(0, 0, _size.width(), _size.height(), grab);

	QPixmap pixmap = grab.copy(_size.width() / _magnificationTimes / 2, _size.height() / _magnificationTimes / 2, 1, 1);
	QColor color = pixmap.toImage().pixel(0, 0);
	if (color.red() > 100 && color.blue() > 100 && color.green() > 100)
		painter.setPen(QColor(0, 0, 0));
	else
		painter.setPen(QColor(255, 255, 255));
	painter.drawRect(0, 0, rect().width() - 1, rect().height() - 1);

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
	QPoint leftop(_size.width() / 2 - _magnificationTimes / 2, _size.height() / 2 - _magnificationTimes / 2);
	painter.drawRect(leftop.x(), leftop.y(), _magnificationTimes, _magnificationTimes);
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
	QPoint position = QCursor::pos() + QPoint(sizeOfMouseIcon, sizeOfMouseIcon);
	if (position.x() + _size.width() > QApplication::desktop()->width())
		position.setX(position.x() - _size.width() - 2 * sizeOfMouseIcon);
	if (position.y() + _size.height() > QApplication::desktop()->height())
		position.setY(position.y() - _size.height() - 2 * sizeOfMouseIcon);
	move(position);
	update();
}