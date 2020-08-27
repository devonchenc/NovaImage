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

    void setData(QGraphicsLineItem* lineItem, const QVector<qreal>& points);

signals:
    void lineWidthChanged(QGraphicsLineItem* plotItem, int lineWidth);

private:
    void initUI();

private slots:
    void deleteLine();

    void setLineWidth();

private:
    QTabWidget* _tabWidget;

    QMap<DiagramPlotItem*, ChartView*> _map;
};
