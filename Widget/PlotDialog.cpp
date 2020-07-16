#include "PlotDialog.h"

#include <QTabWidget>
#include <QVBoxLayout>

#include "../Diagram/DiagramLineItem.h"

PlotDialog::PlotDialog(QWidget* parent)
	: QDialog(parent)
	, _chart(new QChart)
	, _tabWidget(new QTabWidget(this))
{

	setWindowTitle(tr("Plot"));

	setWindowFlag(Qt::Popup);

	initUI();

	resize(500, 300);

	setStyleSheet("background-color:none");
}

PlotDialog::~PlotDialog()
{

}

void PlotDialog::setData(QGraphicsLineItem* lineItem, const QVector<qreal>& points)
{
	DiagramLineItem* item = qgraphicsitem_cast<DiagramLineItem*>(lineItem);
	connect(item, &DiagramLineItem::itemDeleted, this, &PlotDialog::deleteLine);
	_points = points;

	_chart->removeAllSeries();

	QLineSeries* series = new QLineSeries;
	for (int i = 0; i < _points.size(); i++)
	{
		series->append(i, _points[i]);
	}

	_chart->addSeries(series);
	_chart->createDefaultAxes();
	_chart->legend()->hide();

	QChartView* chartView = new QChartView(_chart);
	chartView->setRenderHint(QPainter::Antialiasing);

	_tabWidget->addTab(chartView, "Plot 1");
}

void PlotDialog::initUI()
{
	QVBoxLayout* globalLayout = new QVBoxLayout;
	globalLayout->addWidget(_tabWidget);
	setLayout(globalLayout);
}

void PlotDialog::deleteLine()
{
	int a = 5;
}