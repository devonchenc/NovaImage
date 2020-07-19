#include "PlotDialog.h"

#include <QTabWidget>
#include <QVBoxLayout>

#include "ChartView.h"
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

	QMap<DiagramPlotItem*, ChartView*>::const_iterator iter = _map.find(item);
	if (iter == _map.end())
	{
		// Add new ChartView
		ChartView* chartView = new ChartView;
		chartView->setData(points);

		QString str = QString(tr("Plot")) + QString::number(item->plotIndex());
		_tabWidget->addTab(chartView, str);
		_tabWidget->setCurrentWidget(chartView);

		_map.insert(item, chartView);
	}
	else
	{
		// Update existing plot
		ChartView* chartView = iter.value();
		chartView->updateData(points);

		int index = _tabWidget->indexOf(chartView);
		_tabWidget->setCurrentIndex(index);
	}
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

	QMapIterator<DiagramPlotItem*, ChartView*> i(_map);
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

void PlotDialog::tooltip(const QPointF& point, bool state)
{
//	setToolTip(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
}
