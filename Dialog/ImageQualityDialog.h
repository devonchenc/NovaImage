#pragma once

#include <QDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QGraphicsLineItem;
class QLabel;
QT_END_NAMESPACE

class DiagramImageQualityItem;
class ImageQualityChartView;

class ImageQualityDialog : public QDialog
{
    Q_OBJECT

public:
    ImageQualityDialog(QWidget* parent = nullptr);

    void setData(QGraphicsLineItem* lineItem, const QVector<qreal>& points);

    void setRatio(float leftRatio, float rightRatio);

public slots:
    void updateResult(qreal AHeight, qreal BHeight, qreal CHeight, qreal quality);

signals:
    void lineWidthChanged(QGraphicsLineItem* plotItem, int lineWidth);

private:
    void initUI();

private slots:
    void setLineWidth();

private:
    ImageQualityChartView* _chartView;

    DiagramImageQualityItem* _imageQualityItem;

    QLabel* _aValueLabel;
    QLabel* _bValueLabel;
    QLabel* _cValueLabel;
    QLabel* _resultLabel;
};
