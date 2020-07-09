#include "MagnifierWidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QCursor>
#include <QWindow>
#include <QScreen>
#include <QTimer>

const int grabInterval = 50;
const int magnificationTimes = 10;
const int sizeOfMouseIcon = 20;

MagnifierWidget::MagnifierWidget(QWidget* parent)
	: QWidget(parent)
	, _size(190, 190)
	, _timer(new QTimer)
{	
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
	QPainter painter(this);

	QWindow* window = parentWidget()->windowHandle();
	QPixmap grab = window->screen()->grabWindow(QApplication::desktop()->winId());
	grab = grab.copy(QCursor::pos().x() - _size.width() / magnificationTimes / 2, QCursor::pos().y() - _size.height() / magnificationTimes / 2, _size.width() / magnificationTimes, _size.height() / magnificationTimes);
	painter.drawPixmap(0, 0, _size.width(), _size.height(), grab);

	QPixmap pixmap = grab.copy(_size.width() / magnificationTimes / 2, _size.height() / magnificationTimes / 2, 1, 1);
	QColor color = pixmap.toImage().pixel(0, 0);
	if (color.red() > 100 && color.blue() > 100 && color.green() > 100)
		painter.setPen(QColor(0, 0, 0));
	else
		painter.setPen(QColor(255, 255, 255));
	painter.drawRect(0, 0, rect().width() - 1, rect().height() - 1);

	QPoint leftop(_size.width() / 2 - magnificationTimes / 2, _size.height() / 2 - magnificationTimes / 2);
	painter.drawRect(leftop.x(), leftop.y(), magnificationTimes, magnificationTimes);
}

void MagnifierWidget::updatePosition()
{
	QPoint show = QCursor::pos() + QPoint(sizeOfMouseIcon, -(_size.height() + sizeOfMouseIcon));
	if (show.y() < 0)
		show.setY(show.y() + _size.height() + sizeOfMouseIcon);
	if (show.x() + _size.width() > parentWidget()->width())
		show.setX(show.x() - (_size.height() + sizeOfMouseIcon));
	move(show);
	update();
}