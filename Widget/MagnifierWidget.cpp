#include "MagnifierWidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QCursor>
#include <QWindow>
#include <QScreen>
#include <QTimer>

const QSize winSize(150, 150);
const int grabInterval = 50;
const int magnificationTimes = 10;
const double split = 0.7;
const int sizeOfMouseIcon = 20;

MagnifierWidget::MagnifierWidget(QWidget* parent)
	: QWidget(parent)
	, _timer(new QTimer)
{	
	setFixedSize(winSize);
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
	grab = grab.copy(QCursor::pos().x() - winSize.width() / magnificationTimes / 2, QCursor::pos().y() - winSize.height() * split / magnificationTimes / 2, winSize.width() / magnificationTimes, winSize.height() * split / magnificationTimes);
	painter.drawPixmap(0, 0, winSize.width(), winSize.height() * split, grab);

	QPixmap pixmap = grab.copy(winSize.width() / magnificationTimes / 2, winSize.height() * split / magnificationTimes / 2, 1, 1);
	_color = pixmap.toImage().pixel(0, 0);
	if (_color.red() > 100 && _color.blue() > 100 && _color.green() > 100)
		painter.setPen(QColor(0, 0, 0));
	else
		painter.setPen(QColor(255, 255, 255));
	painter.fillRect(0, winSize.height() * split, winSize.width() - 1, winSize.height() * (1 - split), QColor(100, 100, 100));
	painter.drawRect(0, 0, rect().width() - 1, rect().height() - 1);
	painter.fillRect(4, 74, 22, 22, _color);
	painter.drawLine(50, 30, 50, 40);
	painter.drawLine(45, 35, 55, 35);
	painter.setPen(QColor(255, 255, 255));
	painter.drawText(32, 82, "RGB");
	painter.drawText(32, 95, QString().sprintf("%d,%d,%d", (_color.red() - 1) * 255 / 254, (_color.green() - 1) * 255 / 254, (_color.blue() - 1) * 255 / 254));
}

void MagnifierWidget::updatePosition()
{
	QPoint show = QCursor::pos() + QPoint(sizeOfMouseIcon, -(winSize.height() + sizeOfMouseIcon));
	if (show.y() < 0)
		show.setY(show.y() + winSize.height() + sizeOfMouseIcon);
	if (show.x() + winSize.width() > parentWidget()->width())
		show.setX(show.x() - (winSize.height() + sizeOfMouseIcon));
	move(show);
	update();
}