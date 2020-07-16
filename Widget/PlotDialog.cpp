#include "PlotDialog.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

#include "../Diagram/DiagramPlotItem.h"

PlotDialog::PlotDialog(QWidget* parent)
	: QDialog(parent)
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
	DiagramPlotItem* item = qgraphicsitem_cast<DiagramPlotItem*>(lineItem);
	connect(item, &DiagramPlotItem::itemDeleted, this, &PlotDialog::deleteLine);

	QLineSeries* series = new QLineSeries;
	for (int i = 0; i < points.size(); i++)
	{
		series->append(i, points[i]);
	}

	QChart* chart = new QChart;
	chart->addSeries(series);
	chart->createDefaultAxes();
	chart->legend()->hide();

	QChartView* chartView = new QChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);

	QString str = QString(tr("Plot")) + QString::number(item->plotIndex());
	_tabWidget->addTab(chartView, str);
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