#pragma once

#include "ChartView.h"

class ImageQualityChartView : public ChartView
{
    Q_OBJECT

public:
    ImageQualityChartView(QWidget* parent = nullptr);

    void setData(const QVector<qreal>& points);

    void updateData(const QVector<qreal>& points);

    void setRatio(float leftRatio, float rightRatio);
    void setLeftRatio(float ratio);
    void setRightRatio(float ratio);

public slots:
    void hoverLine(const QPointF& point, bool state);
    void hoverRightLine(const QPointF& point, bool state);

signals:
    void leftRatioChanged(float ratio);
    void rightRatioChanged(float ratio);
    void sendResult(qreal AHeight, qreal BHeight, qreal CHeight, qreal quality);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void addDataPoint(int length);

    void appendConnectionLine();

    void appendABCLine();

private:
    float _leftRatio, _rightRatio;
    bool _dragLeft, _dragRight;
    int _APosition, _BPosition, _CPosition;
    QLineSeries* _leftSeries;
    QLineSeries* _rightSeries;
    QLineSeries* _leftToRightSeries;
    QLineSeries* _ASeries;
    QLineSeries* _BSeries;
    QLineSeries* _CSeries;
    QGraphicsSimpleTextItem* _ALabel;
    QGraphicsSimpleTextItem* _BLabel;
    QGraphicsSimpleTextItem* _CLabel;
};