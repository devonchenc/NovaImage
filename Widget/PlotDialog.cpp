#include "PlotDialog.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QDebug>

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
	if (_tabWidget)
	{
		delete _tabWidget;
		_tabWidget = nullptr;
	}
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
	_tabWidget->setCurrentWidget(chartView);

	_map.insert(item, chartView);
}

void PlotDialog::initUI()
{
	QVBoxLayout* globalLayout = new QVBoxLayout;
	globalLayout->addWidget(_tabWidget);
	setLayout(globalLayout);
}

void PlotDialog::deleteLine()
{
	DiagramPlotItem* item = qobject_cast<DiagramPlotItem*>(sender());

	QMapIterator<DiagramPlotItem*, QChartView*> i(_map);
	while (i.hasNext())
	{
		if (i.next().key() == item)
		{
			int index = _tabWidget->indexOf(i.value());
			_tabWidget->removeTab(index);
			break;
		}
	}
	if (_tabWidget->count() == 0)
	{
		close();
	}
}