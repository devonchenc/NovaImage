#pragma once

#include <QDialog>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

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
//	void acceptButtonClicked();
	void deleteLine();

private:
	QChart* _chart;

	QVector<qreal> _points;

	QTabWidget* _tabWidget;
};
