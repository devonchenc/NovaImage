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

	void zoomIn();

	void zoomOut();

	void zoomNormal();

protected:
	void mouseMoveEvent(QMouseEvent* event) override;

private:
	void applyValue();

private:
	int _value;
	View* _view;
};