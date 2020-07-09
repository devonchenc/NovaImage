#include "Magnifier.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>

#include "MagnifierWidget.h"

Magnifier::Magnifier(QWidget* parent)
    : QWidget(parent)
    , _widget(new MagnifierWidget(this))
{
	resize(QApplication::desktop()->screen()->size());
	setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);
}

void Magnifier::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(255, 255, 255, 1));
}

void Magnifier::mousePressEvent(QMouseEvent*)
{
	if (parentWidget())
	{
		parentWidget()->setCursor(Qt::ArrowCursor);
	}
    close();
}

void Magnifier::mouseReleaseEvent(QMouseEvent*)
{
	if (parentWidget())
	{
		parentWidget()->setCursor(Qt::ArrowCursor);
	}
	close();
}