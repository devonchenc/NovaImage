#include "ToolButton.h"

#include <QPainter>
#include <QMouseEvent>

#include "GlobalFunc.h"
#include "mainwindow.h"
#include "MouseHandler.h"

ToolButton* ToolButton::_leftMouseButton = nullptr;
ToolButton* ToolButton::_rightMouseButton = nullptr;
QVector<ToolButton*> ToolButton::_vec;

ToolButton::ToolButton(QWidget* parent)
	: QToolButton(parent)
	, _currentAction(nullptr)
	, _handler(nullptr)
{
	_vec.append(this);
}

ToolButton::~ToolButton()
{
	if (_handler)
	{
		delete _handler;
		_handler = nullptr;
	}
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
	updateIcon();
}

void ToolButton::setCurrentAction(QAction* action)
{
	_currentAction = action;
}

void ToolButton::setMouseHandler(MouseHandler* handler)
{
	if (_handler)
	{
		_handler->unbounded();
		delete _handler;
	}
	_handler = handler;
}

void ToolButton::setLeftMouseButton(ToolButton* toolButton)
{
	_leftMouseButton = toolButton;
	updateAllButtonsIcon();
}

void ToolButton::unsetLeftMouseButton()
{
	if (_leftMouseButton)
	{
		if (_leftMouseButton->_handler)
		{
			_leftMouseButton->_handler->unbounded();
		}
	}
	updateAllButtonsIcon();
}

ToolButton* ToolButton::leftMouseButton()
{
	return _leftMouseButton;
}

void ToolButton::setRightMouseButton(ToolButton* toolButton)
{
	_rightMouseButton = toolButton;
	updateAllButtonsIcon();
}

void ToolButton::unsetRightMouseButton()
{
	if (_rightMouseButton)
	{
		if (_rightMouseButton->_handler)
		{
			_rightMouseButton->_handler->unbounded();
		}
	}
	updateAllButtonsIcon();
}

ToolButton* ToolButton::rightMouseButton()
{
	return _rightMouseButton;
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
