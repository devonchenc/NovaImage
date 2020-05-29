#include "ToolButton.h"

#include <QPainter>
#include <QMouseEvent>

#include "GlobalFunc.h"
#include "mainwindow.h"

ToolButton* ToolButton::_leftMouseButton = nullptr;
ToolButton* ToolButton::_rightMouseButton = nullptr;
QVector<ToolButton*> ToolButton::_vec;

ToolButton::ToolButton(QWidget* parent)
	: QToolButton(parent)
{
	_vec.append(this);
}

ToolButton::~ToolButton()
{
	for (int i = 0; i < _vec.size(); i++)
	{
		if (this == _vec[i])
		{
			_vec.remove(i);
			break;
		}
	}
}

void ToolButton::setIconByName(const QString& fileName)
{
	_fileName = fileName;
	setIcon(QIcon(fileName));
}

void ToolButton::setLeftMouseButton(ToolButton* toolButton)
{
	_leftMouseButton = toolButton;
}

void ToolButton::setRightMouseButton(ToolButton* toolButton)
{
	_rightMouseButton = toolButton;
}

void ToolButton::mousePressEvent(QMouseEvent* event)
{
	if (event->pos().x() < size().width() - 12)
	{
		if (event->button() == Qt::LeftButton)
		{
			setLeftMouseButton(this);
		}
		else if (event->button() == Qt::RightButton)
		{
			setRightMouseButton(this);
		}
		updateAllButtonsIcon();
	}
}

void ToolButton::updateAllButtonsIcon()
{
	for (int i = 0; i < _vec.size(); i++)
	{
		_vec[i]->updateIcon();
	}
}

void ToolButton::updateIcon()
{
	QIcon icon = QIcon(_fileName);
	if (this != _leftMouseButton && this != _rightMouseButton)
	{
		setIcon(icon);
		return;
	}

	QSize size = getGlobalWindow()->iconSize();
	QPixmap pixmap = icon.pixmap(size.height(), size.height());
	QPainter painter(&pixmap);
	QPen newPen(qRgb(96, 96, 96));
	painter.setPen(QPen(qRgb(96, 96, 96)));
	painter.setBrush(QBrush(qRgb(224, 224, 224)));

	int offset = 1;
	painter.drawRect(QRect(0, 16 + offset, 10, 15 - offset));
	painter.drawRect(QRect(0, 16 + offset, 5, 6));
	painter.drawRect(QRect(5, 16 + offset, 5, 6));

	if (this == _leftMouseButton)
	{
		painter.fillRect(QRect(1, 17 + offset, 4, 5), qRgb(255, 216, 0));
	}
	if (this == _rightMouseButton)
	{
		painter.fillRect(QRect(6, 17 + offset, 4, 5), qRgb(255, 148, 166));
	}
	setIcon(QIcon(pixmap));
}
