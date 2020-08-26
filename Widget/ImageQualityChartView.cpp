#include "ImageQualityChartView.h"

#include <cmath>
#include <QtGui/QMouseEvent>
#include <QValueAxis>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

ImageQualityChartView::ImageQualityChartView(QWidget* parent)
    : ChartView(parent)
    , _leftRate(1 / 3.0f)
    , _rightRate(2 / 3.0f)
    , _dragLeft(false)
    , _dragRight(false)
    , _leftSeries(new QLineSeries)
    , _rightSeries(new QLineSeries)
    , _leftToRightSeries(new QLineSeries)
{
    setRubberBand(QChartView::NoRubberBand);

    connect(_leftSeries, &QLineSeries::hovered, this, &ImageQualityChartView::hoverLine);
    connect(_rightSeries, &QLineSeries::hovered, this, &ImageQualityChartView::hoverLine);
}

ImageQualityChartView::~ImageQualityChartView()
{
    if (_leftSeries)
    {
        delete _leftSeries;
        _leftSeries = nullptr;
    }
    if (_rightSeries)
    {
        delete _rightSeries;
        _rightSeries = nullptr;
    }
    if (_leftToRightSeries)
    {
        delete _leftToRightSeries;
        _leftToRightSeries = nullptr;
    }
}

void ImageQualityChartView::setData(const QVector<qreal>& points)
{
    ChartView::setData(points);

    chart()->setAnimationOptions(QChart::GridAxisAnimations);

    addDataPoint(points.size());
    appendConnectionLine();

    chart()->addSeries(_leftSeries);
    chart()->addSeries(_rightSeries);
    chart()->addSeries(_leftToRightSeries);
    chart()->createDefaultAxes();
}

void ImageQualityChartView::updateData(const QVector<qreal>& points)
{
    ChartView::updateData(points);

    _leftSeries->clear();
    _rightSeries->clear();

    addDataPoint(points.size());
    appendConnectionLine();
}

void ImageQualityChartView::mousePressEvent(QMouseEvent* event)
{
    QPointF leftLine = _leftSeries->at(0);
    QPointF rightLine = _rightSeries->at(0);
    QPointF mousePos = chart()->mapToValue(event->pos());
    if (abs(mousePos.x() - leftLine.x()) < 3)
    {
        _dragLeft = true;
    }
    else if (abs(mousePos.x() - rightLine.x()) < 3)
    {
        _dragRight = true;
    }

    QChartView::mousePressEvent(event);
}

void ImageQualityChartView::mouseMoveEvent(QMouseEvent* event)
{
    QPointF mousePos = chart()->mapToValue(event->pos());

    QList<QAbstractAxis*> axesList = chart()->axes(Qt::Vertical);
    QValueAxis* valueAxis = qobject_cast<QValueAxis*>(axesList[0]);
    if (_dragLeft)
    {
        _leftSeries->clear();
        *_leftSeries << QPointF(mousePos.x(), valueAxis->min()) << QPointF(mousePos.x(), valueAxis->max());
        appendConnectionLine();
        chart()->update();
    }
    else if (_dragRight)
    {
        _rightSeries->clear();
        *_rightSeries << QPointF(mousePos.x(), valueAxis->min()) << QPointF(mousePos.x(), valueAxis->max());
        appendConnectionLine();
    }

    int count = _dataSeries->count();
    QChartView::mouseMoveEvent(event);
}

void ImageQualityChartView::mouseReleaseEvent(QMouseEvent* event)
{
    _dragLeft = false;
    _dragRight = false;

    QChartView::mouseReleaseEvent(event);
}

void ImageQualityChartView::hoverLine(const QPointF& point, bool state)
{
    if (state)
    {
        setCursor(Qt::SizeHorCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

void ImageQualityChartView::hoverRightLine(const QPointF& point, bool state)
{
    if (state)
    {
        setCursor(Qt::SizeHorCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

void ImageQualityChartView::mouseDoubleClickEvent(QMouseEvent* event)
{


    QChartView::mouseDoubleClickEvent(event);
}

void ImageQualityChartView::addDataPoint(int length)
{
    QList<QAbstractAxis*> axesList = chart()->axes(Qt::Vertical);
    QValueAxis* valueAxis = qobject_cast<QValueAxis*>(axesList[0]);

    float leftPos = qMax(length * _leftRate - 1, 0.0f);
    *_leftSeries << QPointF(leftPos, valueAxis->min()) << QPointF(leftPos, valueAxis->max());

    float rightPos = qMax(length * _rightRate - 1, 0.0f);
    *_rightSeries << QPointF(rightPos, valueAxis->min()) << QPointF(rightPos, valueAxis->max());
}

void ImageQualityChartView::appendConnectionLine()
{
    qreal leftPos = _leftSeries->at(0).x();
    qreal rightPos = _rightSeries->at(0).x();

    _leftToRightSeries->clear();

    int x0 = (int)floor(leftPos);
    int x1 = (int)ceil(leftPos);
    qreal lambda_x = leftPos - x0;
    qreal leftY = _dataSeries->at(x0).y() * (1 - lambda_x) + _dataSeries->at(x1).y() * lambda_x;
    x0 = (int)floor(rightPos);
    x1 = (int)ceil(rightPos);
    lambda_x = rightPos - x0;
    qreal rightY = _dataSeries->at(x0).y() * (1 - lambda_x) + _dataSeries->at(x1).y() * lambda_x;
    *_leftToRightSeries << QPointF(leftPos, leftY) << QPointF(rightPos, rightY);
}