#include "ChartView.h"

#include <QtGui/QMouseEvent>
#include <QValueAxis>
#include <QDebug>

ChartView::ChartView(QWidget* parent)
	: QChartView(parent)
	, _dataSeries(new QLineSeries)
	, _hSeries(new QLineSeries)
	, _vSeries(new QLineSeries)
{
	setRenderHint(QPainter::Antialiasing);
	setRubberBand(QChartView::VerticalRubberBand);
	this->chart()->setAnimationOptions(QChart::SeriesAnimations);
}

void ChartView::setData(const QVector<qreal>& points)
{
	qreal minValue = points[0];
	qreal maxValue = points[0];
	for (int i = 0; i < points.size(); i++)
	{
		if (minValue > points[i])
		{
			minValue = points[i];
		}
		if (maxValue < points[i])
		{
			maxValue = points[i];
		}
		_dataSeries->append(i, points[i]);
	}

	QChart* chart = new QChart;
	chart->addSeries(_dataSeries);
	chart->addSeries(_hSeries);
	chart->addSeries(_vSeries);
	chart->createDefaultAxes();
	chart->legend()->hide();

	setChart(chart);
}

void ChartView::updateData(const QVector<qreal>& points)
{
	_dataSeries->clear();
	_hSeries->clear();
	_vSeries->clear();

	qreal minValue = points[0];
	qreal maxValue = points[0];
	for (int i = 0; i < points.size(); i++)
	{
		if (minValue > points[i])
		{
			minValue = points[i];
		}
		if (maxValue < points[i])
		{
			maxValue = points[i];
		}
		_dataSeries->append(i, points[i]);
	}

	QList<QAbstractAxis*> axesList = chart()->axes(Qt::Horizontal);
	for (int i = 0; i < axesList.size(); i++)
	{
		axesList[i]->setRange(0, points.size() - 1);
	}
	axesList = chart()->axes(Qt::Vertical);
	for (int i = 0; i < axesList.size(); i++)
	{
		axesList[i]->setRange(minValue, maxValue);
	}
}

void ChartView::mouseMoveEvent(QMouseEvent* event)
{
	_hSeries->clear();
	_vSeries->clear();

	QPointF position = chart()->mapToValue(event->pos());
	int x0 = (int)floor(position.x());
	int x1 = (int)ceil(position.x());

	int count = _dataSeries->count();
	if (x0 >= 0 && x1 < count)
	{
		qreal lambda_x = position.x() - x0;
		qreal y = _dataSeries->at(x0).y() * (1 - lambda_x) + _dataSeries->at(x1).y() * lambda_x;
		*_hSeries << QPointF(0, y) << QPointF(count - 1, y);

		QList<QAbstractAxis*> axesList = chart()->axes(Qt::Vertical);
		QValueAxis* valueAxis = qobject_cast<QValueAxis*>(axesList[0]);

		*_vSeries << QPointF(position.x(), valueAxis->min()) << QPointF(position.x(), valueAxis->max());
	}

	QChartView::mouseMoveEvent(event);
}

void ChartView::paintEvent(QPaintEvent* event)
{



	QChartView::paintEvent(event);
}