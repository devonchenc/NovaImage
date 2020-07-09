#pragma once

#include <QWidget>

class MagnifierWidget : public QWidget
{
	Q_OBJECT

public:
	explicit MagnifierWidget(QWidget* parent = nullptr);
	~MagnifierWidget();

protected:
	void paintEvent(QPaintEvent*);

public slots:
	void updatePosition();

private:
	QColor _color;

	QTimer* _timer;
};

