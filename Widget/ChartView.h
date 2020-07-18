#pragma once

#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
public:
	ChartView(QWidget* parent = nullptr);

	void setData(const QVector<qreal>& points);

	void updateData(const QVector<qreal>& points);

protected:
	void mouseMoveEvent(QMouseEvent* event) override;

	void paintEvent(QPaintEvent* event) override;

private:
	QLineSeries* _dataSeries;
	QLineSeries* _hSeries;
	QLineSeries* _vSeries;
};