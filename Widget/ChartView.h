#pragma once

#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>

QT_CHARTS_USE_NAMESPACE

class Callout : public QGraphicsItem
{
public:
	Callout(QChart* parent);

	void setText(const QString& text);
	void setAnchor(QPointF point);
	void updateGeometry();

	QRectF boundingRect() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
	QString _text;
	QRectF _textRect;
	QRectF _rect;
	QPointF _anchorPoint;
	QFont _font;
	QChart* _chart;
};

//////////////////////////////////////////////////////////////////////////

class ChartView : public QChartView
{
	Q_OBJECT

public:
	ChartView(QWidget* parent = nullptr);
	~ChartView();

	void setData(const QVector<qreal>& points);

	void updateData(const QVector<qreal>& points);

protected:
	void mouseMoveEvent(QMouseEvent* event) override;

private:
	QLineSeries* _dataSeries;
	QLineSeries* _hSeries;
	QLineSeries* _vSeries;
	Callout* _callout;
};