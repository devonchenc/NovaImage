#include "PlotDialog.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QInputDialog>

#include "ChartView.h"
#include "../Diagram/DiagramPlotItem.h"
#include "../Core/GlobalFunc.h"
#include "../Core/View.h"

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

void PlotDialog::setData(QGraphicsLineItem* lineItem, const QVector<qreal>& points)
{
    DiagramPlotItem* item = qgraphicsitem_cast<DiagramPlotItem*>(lineItem);
    connect(item, &DiagramPlotItem::itemDeleted, this, &PlotDialog::deleteLine);

    QMap<DiagramPlotItem*, ChartView*>::const_iterator iter = _map.find(item);
    if (iter == _map.end())
    {
        // Add new ChartView
        ChartView* chartView = new ChartView(this);
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
    QMenuBar* menuBar = new QMenuBar(this);
    QMenu* menu = menuBar->addMenu(tr("&Settings"));
    QAction* widthAction = menu->addAction(tr("&Line Width"));
    connect(widthAction, &QAction::triggered, this, &PlotDialog::setLineWidth);

    QVBoxLayout* globalLayout = new QVBoxLayout;
    globalLayout->setMenuBar(menuBar);
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

void PlotDialog::setLineWidth()
{
    ChartView* chartView = qobject_cast<ChartView*>(_tabWidget->currentWidget());

    QMapIterator<DiagramPlotItem*, ChartView*> i(_map);
    while (i.hasNext())
    {
        if (i.next().value() == chartView)
        {
            DiagramPlotItem* plotItem = i.key();
            bool ok;
            int lineWidth = QInputDialog::getInt(this, tr("Line Width"), tr("Width:"),
                                                 plotItem->width(), 1, 300, 1, &ok);
            if (ok)
            {
                plotItem->setWidth(lineWidth);
                plotItem->scene()->update();
                emit lineWidthChanged(plotItem, lineWidth);
            }
            break;
        }
    }
}

