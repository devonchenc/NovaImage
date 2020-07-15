#pragma once

#include <QDialog>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

class PlotDialog : public QDialog
{
	Q_OBJECT

public:
	PlotDialog(QWidget* parent = nullptr);
	~PlotDialog();

	void setData(const QVector<qreal>& points);

private:
	void initUI();

private slots:
//	void acceptButtonClicked();

private:
	QChart* _chart;

	QVector<qreal> _points;
};
