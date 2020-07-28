#include "ColorButton.h"

#include <QColorDialog>

ColorButton::ColorButton(const QColor& color, QWidget* parent)
    : QPushButton(parent)
    , _enableColorText(true)
    , _enableToolTip(true)
{
    connect(this, &ColorButton::clicked, this, &ColorButton::slotChangeColor);
    setColor(color);
}

void ColorButton::setColor(const QColor& currentColor)
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

void ColorButton::setText(const QString& text)
{
    if (_realText == text)
        return;

    _realText = text;
    QPushButton::setText(_realText);
}

void ColorButton::enableColorText(bool enableColorText)
{
    if (_enableColorText == enableColorText)
        return;

    _enableColorText = enableColorText;
    if (_enableColorText)
        setColorText(_currentColor.name());
    else
        setText(_realText);
}

void ColorButton::enableToolTip(bool enableToolTip)
{
    if (_enableToolTip == enableToolTip)
        return;

    _enableToolTip = enableToolTip;
    if (_enableToolTip)
        setToolTip(_currentColor.name());
    else
        setToolTip("");
}

void ColorButton::setColorText(const QString& colorText)
{
    if (text() == colorText)
        return;

    QPushButton::setText(colorText);
}

void ColorButton::slotChangeColor()
{
    QColor newColor = QColorDialog::getColor(_currentColor, this);
    if (newColor.isValid())
    {
        setColor(newColor);
    }
}
