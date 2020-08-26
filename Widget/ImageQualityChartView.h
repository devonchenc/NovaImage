#pragma once

#include "ChartView.h"

class ImageQualityChartView : public ChartView
{
    Q_OBJECT

public:
    ImageQualityChartView(QWidget* parent = nullptr);
    ~ImageQualityChartView();

    void setData(const QVector<qreal>& points);

    void updateData(const QVector<qreal>& points);

public slots:
    void hoverLine(const QPointF& point, bool state);
    void hoverRightLine(const QPointF& point, bool state);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void addDataPoint(int length);

    void appendConnectionLine();

private:
    float _leftRate, _rightRate;
    bool _dragLeft, _dragRight;
    QLineSeries* _leftSeries;
    QLineSeries* _rightSeries;
    QLineSeries* _leftToRightSeries;
};