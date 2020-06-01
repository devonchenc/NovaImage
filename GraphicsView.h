#pragma once

#include <QGraphicsView>
#include <QWheelEvent>

#define MAX_ZOOM		300

class View;

class GraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	GraphicsView(View* view, QGraphicsScene* scene, QWidget* parent = nullptr);

protected:
#if QT_CONFIG(wheelevent)
	void wheelEvent(QWheelEvent*) override;
#endif

signals:
	void valueChanged(int value);

	void showCoordinate(const QString& str);

	void showPixelValue(const QString& str);

public slots:
	void setValue(int value);

	void zoomNormal();
	void zoom2x();
	void zoom4x();
	void zoom8x();
	void zoomIn();
	void zoomOut();

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

//	QPointF getCenter() { return _currentCenter; }

private:
	void applyValue();

private:
	View* _view;
	int _zoomFactor;
	bool _isLBtnDown;
	QPoint _mouseLBtnDown;
//	QPointF _currentCenter;
};