#pragma once

#include <QDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QTabWidget;
class QGraphicsLineItem;
QT_END_NAMESPACE

class DiagramPlotItem;
class ChartView;

class PlotDialog : public QDialog
{
	Q_OBJECT

public:
	PlotDialog(QWidget* parent = nullptr);
	~PlotDialog();

	void setData(QGraphicsLineItem* lineItem, const QVector<qreal>& points);

private:
	void initUI();

private slots:
	void deleteLine();

	void tooltip(const QPointF& point, bool state);

private:
	QTabWidget* _tabWidget;

	QMap<DiagramPlotItem*, ChartView*> _map;
};
