#include "ChartView.h"

#include <cmath>
#include <QtGui/QMouseEvent>
#include <QValueAxis>
#include <QGraphicsSceneMouseEvent>

Callout::Callout(QChart* parent)
    : QGraphicsItem(parent)
    , _chart(parent)
{

}

void Callout::setText(const QString& text)
{
    _text = text;
    QFontMetrics metrics(_font);
    _textRect = metrics.boundingRect(QRect(0, 0, 150, 150), Qt::AlignLeft, _text);
    _textRect.translate(5, 5);
    prepareGeometryChange();
    _rect = _textRect.adjusted(-5, -5, 5, 5);
}

void Callout::setAnchor(QPointF point)
{
    _anchorPoint = point;
}

void Callout::updateGeometry()
{
    prepareGeometryChange();
    setPos(_chart->mapToPosition(_anchorPoint) + QPoint(5, -40));
}

QRectF Callout::boundingRect() const
{
    QPointF anchor = mapFromParent(_chart->mapToPosition(_anchorPoint));
    QRectF rect;
    rect.setLeft(qMin(_rect.left(), anchor.x()));
    rect.setRight(qMax(_rect.right(), anchor.x()));
    rect.setTop(qMin(_rect.top(), anchor.y()));
    rect.setBottom(qMax(_rect.bottom(), anchor.y()));
    return rect;
}

void Callout::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    QPainterPath path;
    path.addRoundedRect(_rect, 5, 5);

    QPointF anchor = mapFromParent(_chart->mapToPosition(_anchorPoint));
    if (!_rect.contains(anchor))
    {
        QPointF point1, point2;

        // establish the position of the anchor point in relation to _rect
        bool above = anchor.y() <= _rect.top();
        bool aboveCenter = anchor.y() > _rect.top() && anchor.y() <= _rect.center().y();
        bool belowCenter = anchor.y() > _rect.center().y() && anchor.y() <= _rect.bottom();
        bool below = anchor.y() > _rect.bottom();

        bool onLeft = anchor.x() <= _rect.left();
        bool leftOfCenter = anchor.x() > _rect.left() && anchor.x() <= _rect.center().x();
        bool rightOfCenter = anchor.x() > _rect.center().x() && anchor.x() <= _rect.right();
        bool onRight = anchor.x() > _rect.right();

        // get the nearest m_rect corner
        qreal x = (onRight + rightOfCenter) * _rect.width();
        qreal y = (below + belowCenter) * _rect.height();
        bool cornerCase = (above && onLeft) || (above && onRight) || (below && onLeft) || (below && onRight);
        bool vertical = qAbs(anchor.x() - x) > qAbs(anchor.y() - y);

        qreal x1 = x + leftOfCenter * 10 - rightOfCenter * 20 + cornerCase * !vertical * (onLeft * 10 - onRight * 20);
        qreal y1 = y + aboveCenter * 10 - belowCenter * 20 + cornerCase * vertical * (above * 10 - below * 20);
        point1.setX(x1);
        point1.setY(y1);

        qreal x2 = x + leftOfCenter * 20 - rightOfCenter * 10 + cornerCase * !vertical * (onLeft * 20 - onRight * 10);
        qreal y2 = y + aboveCenter * 20 - belowCenter * 10 + cornerCase * vertical * (above * 20 - below * 10);
        point2.setX(x2);
        point2.setY(y2);

        path.moveTo(point1);
        path.lineTo(anchor);
        path.lineTo(point2);
        path = path.simplified();
    }
    painter->setBrush(QColor(255, 255, 255));
    painter->drawPath(path);
    painter->drawText(_textRect, _text);
}

//////////////////////////////////////////////////////////////////////////

ChartView::ChartView(QWidget* parent)
    : QChartView(parent)
    , _dataSeries(new QLineSeries(this))
    , _hSeries(new QLineSeries(this))
    , _vSeries(new QLineSeries(this))
    , _callout(nullptr)
{
    setRenderHint(QPainter::Antialiasing);
    setRubberBand(QChartView::VerticalRubberBand);
}

bool ChartView::hasValidData()
{
    return chart()->series().size() > 0;
}

void ChartView::setData(const QVector<qreal>& points)
{
    qreal minValue = points[0];
    qreal maxValue = points[0];
    for (int i = 0; i < points.size(); i++)
    {
        if (minValue > points[i])
        {
            minValue = points[i];
        }
        if (maxValue < points[i])
        {
            maxValue = points[i];
        }
        _dataSeries->append(i, points[i]);
    }

    QChart* chart = new QChart;
    chart->addSeries(_dataSeries);
    chart->addSeries(_hSeries);
    chart->addSeries(_vSeries);
    chart->createDefaultAxes();
    QList<QAbstractAxis*> axesList = chart->axes(Qt::Horizontal);
    for (int i = 0; i < axesList.size(); i++)
    {
        axesList[i]->setRange(0, points.size() - 1);
    }
    axesList = chart->axes(Qt::Vertical);
    for (int i = 0; i < axesList.size(); i++)
    {
        axesList[i]->setRange(minValue, maxValue);
    }
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::AllAnimations);

    setChart(chart);
}

void ChartView::updateData(const QVector<qreal>& points)
{
    _dataSeries->clear();
    _hSeries->clear();
    _vSeries->clear();

    qreal minValue = points[0];
    qreal maxValue = points[0];
    for (int i = 0; i < points.size(); i++)
    {
        if (minValue > points[i])
        {
            minValue = points[i];
        }
        if (maxValue < points[i])
        {
            maxValue = points[i];
        }
        _dataSeries->append(i, points[i]);
    }

    QList<QAbstractAxis*> axesList = chart()->axes(Qt::Horizontal);
    for (int i = 0; i < axesList.size(); i++)
    {
        axesList[i]->setRange(0, points.size() - 1);
    }
    axesList = chart()->axes(Qt::Vertical);
    for (int i = 0; i < axesList.size(); i++)
    {
        axesList[i]->setRange(minValue, maxValue);
    }
}

void ChartView::mouseMoveEvent(QMouseEvent* event)
{
    _hSeries->clear();
    _vSeries->clear();

    QPointF position = chart()->mapToValue(event->pos());
    int x0 = (int)floor(position.x());
    int x1 = (int)ceil(position.x());

    int count = _dataSeries->count();
    if (x0 >= 0 && x1 < count)
    {
        qreal lambda_x = position.x() - x0;
        qreal y = _dataSeries->at(x0).y() * (1 - lambda_x) + _dataSeries->at(x1).y() * lambda_x;
        *_hSeries << QPointF(0, y) << QPointF(count - 1, y);

        QList<QAbstractAxis*> axesList = chart()->axes(Qt::Vertical);
        QValueAxis* valueAxis = qobject_cast<QValueAxis*>(axesList[0]);

        *_vSeries << QPointF(position.x(), valueAxis->min()) << QPointF(position.x(), valueAxis->max());

        // Set pen style
        QPen pen = _hSeries->pen();
        pen.setStyle(Qt::DashLine);
        _hSeries->setPen(pen);
        pen = _vSeries->pen();
        pen.setStyle(Qt::DashLine);
        _vSeries->setPen(pen);

        if (_callout == nullptr)
            _callout = new Callout(chart());

        qreal value = _hSeries->at(0).y();
        int decimal = 4;
        if (abs(value) > 10.0f)
        {
            decimal = 1;
        }
        else if (abs(value) > 1.0f)
        {
            decimal = 2;
        }
        else if (abs(value) > 0.1f)
        {
            decimal = 3;
        }
        _callout->setText(QString::number(value, 'f', decimal));
        _callout->setAnchor(QPointF(_vSeries->at(0).x(), value));
        _callout->setZValue(11);
        _callout->updateGeometry();
        _callout->show();
    }
    else
    {
        if (_callout)
            _callout->hide();
    }

    QChartView::mouseMoveEvent(event);
}
