#pragma once

#include <QPushButton>

class QColorButton : public QPushButton
{
    Q_OBJECT

public:
    QColorButton(const QColor& color = Qt::blue, QWidget* parent = nullptr);

    void setColor(const QColor& color);

    inline QColor getColor() const { return _currentColor; }

    void setText(const QString& text);

    void enableColorText(bool enableColorText);

    inline bool isColorTextEnabled() const { return _enableColorText; }

    void enableToolTip(bool enableToolTip);

    inline bool isToolTipEnabled() const { return _enableToolTip; }

private:
    void setColorText(const QString& colorText);


private:
    bool _enableColorText;
    bool _enableToolTip;

    QColor _currentColor;

    QString _realText;

private slots:
    void slotChangeColor();
};
