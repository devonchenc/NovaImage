#include "ImageQualityChartView.h"

#include <cmath>
#include <float.h>
#include <QtGui/QMouseEvent>
#include <QValueAxis>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

ImageQualityChartView::ImageQualityChartView(QWidget* parent)
    : ChartView(parent)
    , _leftRatio(1 / 3.0f)
    , _rightRatio(2 / 3.0f)
    , _dragLeft(false)
    , _dragRight(false)
    , _APosition(0)
    , _BPosition(0)
    , _CPosition(0)
    , _leftSeries(new QLineSeries(this))
    , _rightSeries(new QLineSeries(this))
    , _leftToRightSeries(new QLineSeries(this))
    , _ASeries(new QLineSeries(this))
    , _BSeries(new QLineSeries(this))
    , _CSeries(new QLineSeries(this))
    , _ALabel(nullptr)
    , _BLabel(nullptr)
    , _CLabel(nullptr)
{
    setRubberBand(QChartView::NoRubberBand);

    connect(_leftSeries, &QLineSeries::hovered, this, &ImageQualityChartView::hoverLine);
    connect(_rightSeries, &QLineSeries::hovered, this, &ImageQualityChartView::hoverLine);
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

    chart()->addSeries(_ASeries);
    chart()->addSeries(_BSeries);
    chart()->addSeries(_CSeries);

    // Recreate axes
    QList<QAbstractAxis*> axesList = chart()->axes(Qt::Vertical);
    QValueAxis* valueAxis = qobject_cast<QValueAxis*>(axesList[0]);
    qreal minValue = valueAxis->min();
    qreal maxValue = valueAxis->max();
    chart()->createDefaultAxes();

    axesList = chart()->axes(Qt::Horizontal);
    for (int i = 0; i < axesList.size(); i++)
    {
        axesList[i]->setRange(0, points.size() - 1);
    }
    axesList = chart()->axes(Qt::Vertical);
    for (int i = 0; i < axesList.size(); i++)
    {
        axesList[i]->setRange(minValue, maxValue);
    }

    _ALabel = new QGraphicsSimpleTextItem(chart());
    _ALabel->setText("A");
    _BLabel = new QGraphicsSimpleTextItem(chart());
    _BLabel->setText("B");
    _CLabel = new QGraphicsSimpleTextItem(chart());
    _CLabel->setText("C");
    _ALabel->hide();
    _BLabel->hide();
    _CLabel->hide();
}

void ImageQualityChartView::updateData(const QVector<qreal>& points)
{
    ChartView::updateData(points);

    addDataPoint(points.size());
    appendConnectionLine();

    _ASeries->clear();
    _BSeries->clear();
    _CSeries->clear();
    _ALabel->hide();
    _BLabel->hide();
    _CLabel->hide();
}

void ImageQualityChartView::setRatio(float leftRatio, float rightRatio)
{
    _leftRatio = leftRatio;
    _rightRatio = rightRatio;
}

void ImageQualityChartView::setLeftRatio(float ratio)
{
    _leftRatio = ratio;
}

void ImageQualityChartView::setRightRatio(float ratio)
{
    _rightRatio = ratio;
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
    if (mousePos.x() >=0 && mousePos.x() < _dataSeries->count() - 1)
    {
        QList<QAbstractAxis*> axesList = chart()->axes(Qt::Vertical);
        QValueAxis* valueAxis = qobject_cast<QValueAxis*>(axesList[0]);
        if (_dragLeft)
        {
            _leftSeries->clear();
            _ASeries->clear();
            _BSeries->clear();
            _CSeries->clear();

            *_leftSeries << QPointF(mousePos.x(), valueAxis->min()) << QPointF(mousePos.x(), valueAxis->max());
            appendConnectionLine();

            _leftRatio = mousePos.x() / (_dataSeries->count() - 1);
            emit leftRatioChanged(_leftRatio);
        }
        else if (_dragRight)
        {
            _rightSeries->clear();
            _ASeries->clear();
            _BSeries->clear();
            _CSeries->clear();

            *_rightSeries << QPointF(mousePos.x(), valueAxis->min()) << QPointF(mousePos.x(), valueAxis->max());
            appendConnectionLine();

            _rightRatio = mousePos.x() / (_dataSeries->count() - 1);
            emit rightRatioChanged(_rightRatio);
        }
    }

    QChartView::mouseMoveEvent(event);
}

void ImageQualityChartView::mouseReleaseEvent(QMouseEvent* event)
{
    _dragLeft = false;
    _dragRight = false;

    QChartView::mouseReleaseEvent(event);
}

void ImageQualityChartView::hoverLine(const QPointF&, bool state)
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

void ImageQualityChartView::hoverRightLine(const QPointF&, bool state)
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
    qreal leftPos = _leftSeries->at(0).x();
    qreal rightPos = _rightSeries->at(0).x();
    int left = (int)round(qMin(leftPos, rightPos));
    int right = (int)round(qMax(leftPos, rightPos));
    int objectionNum = abs(right - left) + 1;

    // Find minimum value
    qreal minValue1 = _dataSeries->at(left).y();
    int nAPos = 0;
    for (int i = 0; i < objectionNum; i++)
    {
        if (minValue1 > _dataSeries->at(i + left).y())
        {
            minValue1 = _dataSeries->at(i + left).y();
            nAPos = i;
        }
    }
    _APosition = nAPos + left;

    bool* pSearchedFlag = new bool[objectionNum];
    memset(pSearchedFlag, 0, sizeof(bool) * objectionNum);
    // Set the minimum point mark to true
    pSearchedFlag[nAPos] = true;

    // Start searching near the minimum
    qreal tempValue = minValue1;
    for (int i = nAPos - 1; i >= 0; i--)
    {
        // If the value to be searched is greater than tempValue
        // mark that the value has been searched
        if (_dataSeries->at(i + left).y() >= tempValue)
        {
            pSearchedFlag[i] = true;
            tempValue = _dataSeries->at(i + left).y();
        }
        else
        {
            break;
        }
    }
    tempValue = minValue1;
    for (int i = nAPos + 1; i < objectionNum; i++)
    {
        if (_dataSeries->at(i + left).y() >= tempValue)
        {
            pSearchedFlag[i] = true;
            tempValue = _dataSeries->at(i + left).y();
        }
        else
        {
            break;
        }
    }

    bool continueSearch = false;
    for (int i = 0; i < objectionNum; i++)
    {
        if (pSearchedFlag[i] == false)
        {
            continueSearch = true;
            break;
        }
    }
    if (!continueSearch)
    {
        // This means there is only one peak
        _APosition = _BPosition = _CPosition = 0;
        delete[] pSearchedFlag;
        return;
    }

    // Find the minimum value among the remaining points
    float minValue2 = FLT_MAX;
    int nBPos = 0;
    for (int i = 0; i < objectionNum; i++)
    {
        if (minValue2 > _dataSeries->at(i + left).y() && pSearchedFlag[i] == false)
        {
            minValue2 = _dataSeries->at(i + left).y();
            nBPos = i;
        }
    }
    _BPosition = nBPos + left;

    delete[] pSearchedFlag;

    // Find the maximum value among points A and B
    float fLocalMaxValue = -FLT_MAX;
    int nCPos = 0;
    for (int i = qMin(nAPos, nBPos); i <= qMax(nAPos, nBPos); i++)
    {
        if (fLocalMaxValue < _dataSeries->at(i + left).y())
        {
            fLocalMaxValue = _dataSeries->at(i + left).y();
            nCPos = i;
        }
    }
    _CPosition = nCPos + left;

    appendABCLine();

    ChartView::mouseDoubleClickEvent(event);
}

void ImageQualityChartView::resizeEvent(QResizeEvent* event)
{
    QPointF ABottom(_APosition, _dataSeries->at(_APosition).y());
    QPointF BBottom(_BPosition, _dataSeries->at(_BPosition).y());
    QPointF CBottom(_CPosition, _dataSeries->at(_CPosition).y());

    _ALabel->setPos(chart()->mapToPosition(ABottom));
    _BLabel->setPos(chart()->mapToPosition(BBottom));
    _CLabel->setPos(chart()->mapToPosition(CBottom));

    ChartView::resizeEvent(event);
}

void ImageQualityChartView::addDataPoint(int length)
{
    _leftSeries->clear();
    _rightSeries->clear();

    QList<QAbstractAxis*> axesList = chart()->axes(Qt::Vertical);
    QValueAxis* valueAxis = qobject_cast<QValueAxis*>(axesList[0]);

    float leftPos = _leftRatio * (length - 1);
    *_leftSeries << QPointF(leftPos, valueAxis->min()) << QPointF(leftPos, valueAxis->max());

    float rightPos = _rightRatio * (length - 1);
    *_rightSeries << QPointF(rightPos, valueAxis->min()) << QPointF(rightPos, valueAxis->max());
}

void ImageQualityChartView::appendConnectionLine()
{
    _leftToRightSeries->clear();

    qreal leftPos = _leftSeries->at(0).x();
    qreal rightPos = _rightSeries->at(0).x();

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

void ImageQualityChartView::appendABCLine()
{
    _ASeries->clear();
    _BSeries->clear();
    _CSeries->clear();

    if (_CPosition == 0)
        return;

    QPointF leftPoint = _leftToRightSeries->at(0);
    QPointF rightPoint = _leftToRightSeries->at(1);

    qreal ratio = (_APosition - leftPoint.x()) / (rightPoint.x() - leftPoint.x());
    QPointF ATop = QPointF(_APosition, leftPoint.y() * (1 - ratio) + rightPoint.y() * ratio);
    QPointF ABottom(_APosition, _dataSeries->at(_APosition).y());
    *_ASeries << ABottom << ATop;

    ratio = (_BPosition - leftPoint.x()) / (rightPoint.x() - leftPoint.x());
    QPointF BTop = QPointF(_BPosition, leftPoint.y() * (1 - ratio) + rightPoint.y() * ratio);
    QPointF BBottom(_BPosition, _dataSeries->at(_BPosition).y());
    *_BSeries << BBottom << BTop;

    ratio = (_CPosition - leftPoint.x()) / (rightPoint.x() - leftPoint.x());
    QPointF CTop = QPointF(_CPosition, leftPoint.y() * (1 - ratio) + rightPoint.y() * ratio);
    QPointF CBottom(_CPosition, _dataSeries->at(_CPosition).y());
    *_CSeries << CBottom << CTop;

    qreal AHeight = chart()->mapToPosition(ABottom).y() - chart()->mapToPosition(ATop).y();
    qreal BHeight = chart()->mapToPosition(BBottom).y() - chart()->mapToPosition(BTop).y();
    qreal CHeight = chart()->mapToPosition(CBottom).y() - chart()->mapToPosition(CTop).y();
    qreal quality = (AHeight + BHeight - 2 * CHeight) / (AHeight + BHeight) * 100.0;

    emit sendResult(AHeight, BHeight, CHeight, quality);

    _ALabel->setPos(chart()->mapToPosition(ABottom));
    _BLabel->setPos(chart()->mapToPosition(BBottom));
    _CLabel->setPos(chart()->mapToPosition(CBottom));
    _ALabel->show();
    _BLabel->show();
    _CLabel->show();
}
