#pragma once

#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>

QT_CHARTS_USE_NAMESPACE

class Callout : public QGraphicsItem
{
public:
    Callout(QChart* parent);

    void setText(const QString& text);
    void setAnchor(QPointF point);
    void updateGeometry();

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QString _text;
    QRectF _textRect;
    QRectF _rect;
    QPointF _anchorPoint;
    QFont _font;
    QChart* _chart;
};

//////////////////////////////////////////////////////////////////////////

class ChartView : public QChartView
{
    Q_OBJECT

public:
    ChartView(QWidget* parent = nullptr);
    ~ChartView();

    bool hasValidData();

    void setData(const QVector<qreal>& points);

    void updateData(const QVector<qreal>& points);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;

protected:
    QLineSeries* _dataSeries;
    QLineSeries* _hSeries;
    QLineSeries* _vSeries;
    Callout* _callout;
};

//////////////////////////////////////////////////////////////////////////

class ChartView2 : public ChartView
{
    Q_OBJECT

public:
    ChartView2(QWidget* parent = nullptr);
    ~ChartView2();

    void setData(const QVector<qreal>& points);

    void updateData(const QVector<qreal>& points);

public slots:
    void hoverLine(const QPointF& point, bool state);
    void hoverRightLine(const QPointF& point, bool state);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    float _leftRate, _rightRate;
    bool _dragLeft, _dragRight;
    QLineSeries* _leftSeries;
    QLineSeries* _rightSeries;
};