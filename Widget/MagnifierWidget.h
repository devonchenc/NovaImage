#pragma once

#include <QWidget>

class MagnifierWidget : public QWidget
{
	Q_OBJECT

public:
	explicit MagnifierWidget(QWidget* parent = nullptr);
	~MagnifierWidget();

protected:
	void paintEvent(QPaintEvent*) override;

	void mousePressEvent(QMouseEvent*) override;
	void mouseMoveEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;

public slots:
	void updatePosition();

private:
	QSize _size;
	QTimer* _timer;
};
