#pragma once

#include <QDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QGraphicsLineItem;
QT_END_NAMESPACE

class DiagramImageQualityItem;
class ChartView;

class ImageQualityDialog : public QDialog
{
    Q_OBJECT

public:
    ImageQualityDialog(QWidget* parent = nullptr);
    ~ImageQualityDialog();

    void setData(QGraphicsLineItem* lineItem, const QVector<qreal>& points);

signals:
    void lineWidthChanged(QGraphicsLineItem* plotItem, int lineWidth);

private:
    void initUI();

private slots:
    void setLineWidth();

private:
    ChartView* _chartView;

    DiagramImageQualityItem* _imageQualityItem;
};
