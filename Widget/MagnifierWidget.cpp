#include "MagnifierWidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QCursor>
#include <QWindow>
#include <QScreen>
#include <QTimer>
#include <QDebug>

const int grabInterval = 50;
const int magnificationTimes = 10;
const int sizeOfMouseIcon = 20;

MagnifierWidget::MagnifierWidget(QWidget* parent)
	: QWidget(parent)
	, _size(190, 190)
	, _timer(new QTimer)
{
	setWindowFlags(Qt::Popup);
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
	grab = grab.copy(QCursor::pos().x() - _size.width() / magnificationTimes / 2, QCursor::pos().y() - _size.height() / magnificationTimes / 2,
		_size.width() / magnificationTimes, _size.height() / magnificationTimes);

	QPainter painter(this);
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

void MagnifierWidget::mousePressEvent(QMouseEvent*)
{
	if (parentWidget())
	{
		qDebug() << " MagnifierWidget::mousePressEvent";
		parentWidget()->setCursor(Qt::ArrowCursor);
	}
	close();
}

void MagnifierWidget::mouseMoveEvent(QMouseEvent* event)
{
	qDebug() << " MagnifierWidget::mouseMoveEvent";
//	this->parentWidget()->mouseMoveEvent(event);
}

void MagnifierWidget::mouseReleaseEvent(QMouseEvent*)
{
	if (parentWidget())
	{
		qDebug() << " MagnifierWidget::mouseReleaseEvent";
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