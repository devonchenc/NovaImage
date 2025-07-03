#include "DiagramPlotItem.h"

#include <cmath>
#include <QPainter>
#include <QDomDocument>
#include <QGraphicsOpacityEffect>

#include "../Core/GlobalFunc.h"
#include "../Core/View.h"
#include "../Core/GraphicsView.h"

int DiagramPlotItem::_plotCount = 0;

DiagramPlotItem::DiagramPlotItem(QGraphicsItem* parent)
    : DiagramLineItem(QLineF(), nullptr, parent)
    , _lineWidth(1)
{
    _plotCount++;
    _plotIndex = _plotCount;
}

DiagramPlotItem::DiagramPlotItem(const QLineF& line, QMenu* contextMenu, QGraphicsItem* parent)
    : DiagramLineItem(line, contextMenu, parent)
    , _lineWidth(1)
{
    _plotCount++;
    _plotIndex = _plotCount;
}

DiagramPlotItem::~DiagramPlotItem()
{
    emit itemDeleted();
}

QDomElement DiagramPlotItem::saveToXML(QDomDocument& doc) const
{
    QDomElement lineItem = doc.createElement("GraphicsItem");
    lineItem.setAttribute("Type", "DiagramLineItem");

    QDomElement attribute = doc.createElement("Attribute");
    attribute.setAttribute("Name", "Plot");
    attribute.setAttribute("Position", pointFToString(pos()));
    attribute.setAttribute("Point1", pointFToString(line().p1()));
    attribute.setAttribute("Point2", pointFToString(line().p2()));
    attribute.setAttribute("Color", colorToString(pen().color()));
    attribute.setAttribute("EndPointColor", colorToString(_endpointPen.color()));
    attribute.setAttribute("LineWidth", QString::number(_lineWidth));
    attribute.setAttribute("Index", QString::number(_plotIndex));
    attribute.setAttribute("Opacity", QString::number(_effect->opacity(), 'f', 2));

    lineItem.appendChild(attribute);
    return lineItem;
}

void DiagramPlotItem::loadFromXML(const QDomElement& e)
{
    DiagramLineItem::loadFromXML(e);

    _lineWidth = e.attribute("LineWidth").toInt();
    _plotIndex = e.attribute("Index").toInt();

    _plotCount = qMax(_plotCount, _plotIndex);

    View* view = qobject_cast<View*>(scene()->parent());
    if (view)
    {
        view->showPlotDialog(this);
    }
}

void DiagramPlotItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    DiagramLineItem::paint(painter, option, widget);

    drawRect(painter);
    drawResizeHandle(painter);
    drawPlotIndex(painter);
}

void DiagramPlotItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    getGlobalActiveView()->showPlotDialog(this);

    DiagramLineItem::mouseReleaseEvent(event);
}

void DiagramPlotItem::drawRect(QPainter* painter)
{
    if (_lineWidth <= 1)
        return;

    QPointF p1 = line().p1();
    QPointF p2 = line().p2();
    QPointF slope = line().p2() - line().p1();
    QPointF orthoSlope(-slope.y(), slope.x());
    if (orthoSlope.x() != 0.0f || orthoSlope.y() != 0.0f)
    {
        orthoSlope /= sqrt(orthoSlope.x() * orthoSlope.x() + orthoSlope.y() * orthoSlope.y());
    }

    QVector<QPointF> vec;
    vec.push_back(p1 - orthoSlope * _lineWidth / 2.0);
    vec.push_back(p1 + orthoSlope * _lineWidth / 2.0);
    vec.push_back(p2 + orthoSlope * _lineWidth / 2.0);
    vec.push_back(p2 - orthoSlope * _lineWidth / 2.0);

    QPolygonF polygon(vec);
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(QBrush(QColor(128, 128, 255, 128)));
    painter->drawConvexPolygon(polygon);

    painter->setRenderHint(QPainter::Antialiasing, false);
}

void DiagramPlotItem::drawPlotIndex(QPainter* painter)
{
    QGraphicsScene* graphicsScene = scene();
    QList<QGraphicsView*> viewList = graphicsScene->views();
    Q_ASSERT(viewList.size() > 0);

    QTransform transform = viewList.first()->transform();
    QTransform transform2;
    transform2.translate(line().p2().x() + 10, line().p2().y() + 5);

    painter->setWorldTransform(transform.inverted() * transform2, true);
    painter->setFont(QFont("Arial", 10));
    painter->setPen(QPen(Qt::yellow));
    painter->drawText(0, 0, QString::number(_plotIndex));
}
