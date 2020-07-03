#include "ColorButton.h"

#include <QColorDialog>

QColorButton::QColorButton(const QColor& color, QWidget* parent)
	: QPushButton(parent)
	, _enableColorText(true)
	, _enableToolTip(true)
{
	connect(this, &QColorButton::clicked, this, &QColorButton::slotChangeColor);
	setColor(color);
}

void QColorButton::setColor(const QColor& currentColor)
{
	if (_currentColor == currentColor)
		return;

	_currentColor = currentColor;
	if (_enableColorText)
		setColorText(_currentColor.name());
	if (_enableToolTip)
		setToolTip(_currentColor.name());

	QPixmap pix(16, 16);
	pix.fill(_currentColor);
	setIcon(pix);
}

void QColorButton::setText(const QString& text)
{
	if (_realText == text)
		return;

	_realText = text;
	QPushButton::setText(_realText);
}

void QColorButton::enableColorText(bool enableColorText)
{
	if (_enableColorText == enableColorText)
		return;

	_enableColorText = enableColorText;
	if (_enableColorText)
		setColorText(_currentColor.name());
	else
		setText(_realText);
}

void QColorButton::enableToolTip(bool enableToolTip)
{
	if (_enableToolTip == enableToolTip)
		return;

	_enableToolTip = enableToolTip;
	if (_enableToolTip)
		setToolTip(_currentColor.name());
	else
		setToolTip("");
}

void QColorButton::setColorText(const QString& colorText)
{
	if (text() == colorText)
		return;

	QPushButton::setText(colorText);
}

void QColorButton::slotChangeColor()
{
	QColor newColor = QColorDialog::getColor(_currentColor, this);
	if (newColor.isValid())
	{
		setColor(newColor);
	}
}