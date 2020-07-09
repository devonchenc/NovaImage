#pragma once

#include <QWidget>

class MagnifierWidget;

class Magnifier : public QWidget
{
    Q_OBJECT

public:
    explicit Magnifier(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent*);

	void mousePressEvent(QMouseEvent*) override;

	void mouseReleaseEvent(QMouseEvent*) override;

private:
	MagnifierWidget* _widget;
};
