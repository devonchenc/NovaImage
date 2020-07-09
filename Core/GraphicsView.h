#pragma once

#include <QGraphicsView>
#include <QWheelEvent>

#define MAX_ZOOM		1200
#define ZOOM_STEP		200

class View;
class Magnifier;

class GraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	GraphicsView(View* view, QGraphicsScene* scene, QWidget* parent = nullptr);

	QPointF mapImagePointToScene(qreal x, qreal y) const;

	void setZoomValue(int value);

	void setZoomValueOffset(int offset);

	void showAnnotation(bool show);

	void showMagnifier();

protected:
	void wheelEvent(QWheelEvent*) override;

	void dragEnterEvent(QDragEnterEvent* event) override;

public slots:
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

	void paintEvent(QPaintEvent* event) override;

private:
	void applyZoomValue();

private:
	View* _view;
	int _zoomFactor;
	QString _strCoord;
	QString _strValue;
	bool _showAnnotation;
	Magnifier* _magnifier;
};