#include "PlotDialog.h"

#include <QVBoxLayout>

PlotDialog::PlotDialog(QWidget* parent)
	: QDialog(parent)
	, _chart(new QChart)
{
	setWindowTitle(tr("Plot"));

	setWindowFlag(Qt::Popup);
	
	initUI();

	resize(400, 200);

	setStyleSheet("background-color:none");
}

PlotDialog::~PlotDialog()
{

}

void PlotDialog::setData(const QVector<qreal>& points)
{
	_points = points;

	_chart->removeAllSeries();

	QLineSeries* series = new QLineSeries;
	for (int i = 0; i < _points.size(); i++)
	{
		series->append(i, _points[i]);
	}

	_chart->addSeries(series);
	_chart->createDefaultAxes();
}

void PlotDialog::initUI()
{
	_chart->legend()->hide();

	QChartView* chartView = new QChartView(_chart);
	chartView->setRenderHint(QPainter::Antialiasing);

	QVBoxLayout* globalLayout = new QVBoxLayout;
	globalLayout->addWidget(chartView);
	setLayout(globalLayout);
}