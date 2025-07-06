#include "DiagramPolygonItem.h"

#include <cmath>
#include <float.h>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsOpacityEffect>
#include <QtGlobal>
#include <QtMath>
#include <QDebug>

#include "../Core/GlobalFunc.h"
#include "../Core/View.h"
#include "../Core/GraphicsView.h"
#include "../Core/GraphicsScene.h"
#include "../Image/BaseImage.h"

DiagramPolygonItem::DiagramPolygonItem(QGraphicsItem* parent)
    : QGraphicsPolygonItem(parent)
    , _diagramType(None)
    , _contextMenu(nullptr)
    , _previousMode(MOVE_ITEM)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);

    _effect = new QGraphicsOpacityEffect(this);
    _effect->setOpacity(1.0);
    setGraphicsEffect(_effect);
}

DiagramPolygonItem::DiagramPolygonItem(DiagramType diagramType, QMenu* contextMenu, QGraphicsItem* parent)
    : QGraphicsPolygonItem(parent)
    , _diagramType(diagramType)
    , _contextMenu(contextMenu)
    , _previousMode(MOVE_ITEM)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);

    _effect = new QGraphicsOpacityEffect(this);
    _effect->setOpacity(1.0);
    setGraphicsEffect(_effect);
}

void DiagramPolygonItem::setRectF(const QRectF& rect)
{
    _polygon.clear();

    int width = rect.width();
    int height = rect.height();

    QPainterPath path;
    switch (_diagramType)
    {
    case Rect:
        _polygon << rect.topLeft() << rect.topRight() << rect.bottomRight() << rect.bottomLeft() << rect.topLeft();
        break;
    case RoundRect:
        path.moveTo(rect.right(), rect.center().y());
        path.arcTo(rect.right() - width / 4, rect.top(), width / 4, height / 4, 0, 90);
        path.arcTo(rect.left(), rect.top(), width / 4, height / 4, 90, 90);
        path.arcTo(rect.left(), rect.bottom() - height / 4, width / 4, height / 4, 180, 90);
        path.arcTo(rect.right() - width / 4, rect.bottom() - height / 4, width / 4, height / 4, 270, 90);
        path.lineTo(rect.right(), rect.center().y());
        _polygon = path.toFillPolygon();
        break;
    case Circle:
        path.addEllipse(rect.left(), rect.top(), qMin(width, height), qMin(width, height));
        _polygon = path.toFillPolygon();
        break;
    case Ellipse:
        path.addEllipse(rect);
        _polygon = path.toFillPolygon();
        break;
    case Rhombus:
        _polygon << QPointF(rect.left(), rect.center().y()) << QPointF(rect.center().x(), rect.bottom())
            << QPointF(rect.right(), rect.center().y()) << QPointF(rect.center().x(), rect.top())
            << QPointF(rect.left(), rect.center().y());
        break;
    case Parallelogram:
        _polygon << QPointF(rect.topLeft()) << QPointF(rect.left() + width / 4, rect.bottom())
            << QPointF(rect.bottomRight()) << QPointF(rect.right() - width / 4, rect.top())
            << QPointF(rect.topLeft());
        break;
    case None:
    case Text:
    case Line:
    case Arrow:
    case Plot:
    case ImageQuality:
    case Angle:
        break;
    }
    setPolygon(_polygon);
}

void DiagramPolygonItem::setTransparency(int value)
{
    _effect->setOpacity(1.0 - float(value) / 100);
}

int DiagramPolygonItem::transparency() const
{
    return 100 - round(_effect->opacity() * 100);
}

void DiagramPolygonItem::setDrawingFinished(bool finished)
{
    _drawingFinished = finished;
}

QDomElement DiagramPolygonItem::saveToXML(QDomDocument& doc) const
{
    QDomElement lineItem = doc.createElement("GraphicsItem");
    lineItem.setAttribute("Type", "DiagramItem");

    QDomElement attribute = doc.createElement("Attribute");
    attribute.setAttribute("DiagramType", QString::number(_diagramType));
    attribute.setAttribute("Position", pointFToString(pos()));
    attribute.setAttribute("Polygon", polygonToString(_polygon));
    attribute.setAttribute("Info", _info);
    attribute.setAttribute("PenColor", colorToString(pen().color()));
    attribute.setAttribute("BrushColor", colorToString(brush().color()));
    attribute.setAttribute("BrushStyle", QString::number(brush().style()));
    attribute.setAttribute("Opacity", QString::number(_effect->opacity(), 'f', 2));

    lineItem.appendChild(attribute);
    return lineItem;
}

void DiagramPolygonItem::loadFromXML(const QDomElement& e)
{
    setPos(stringToPointF(e.attribute("Position")));

    _diagramType = DiagramType(e.attribute("DiagramType").toInt());
    QString strPolygon = e.attribute("Polygon");
    _polygon = stringToPolygon(strPolygon);
    setPolygon(_polygon);

    _info = e.attribute("Info");

    QColor color = stringToColor(e.attribute("PenColor"));
    setPen(QPen(color, 2));

    color = stringToColor(e.attribute("BrushColor"));
    Qt::BrushStyle style = Qt::BrushStyle(e.attribute("BrushStyle").toInt());
    QBrush brush(color);
    brush.setStyle(style);
    setBrush(brush);

    _effect->setOpacity(e.attribute("Opacity").toDouble());

    _drawingFinished = true;
}

void DiagramPolygonItem::statisticsInfo()
{
    BaseImage* image = getGlobalImage();
    if (image == nullptr)
        return;

    float sum = 0;
    uint totalPixelCount = 0;
    float minValue = FLT_MAX;
    float maxValue = -FLT_MAX;
    float sumOfSquares = 0;

    QVector<float> vecValue;

    QRectF rect = sceneBoundingRect();
    for (int j = 0; j < rect.height(); j++)
    {
        for (int i = 0; i < rect.width(); i++)
        {
            int x = i + rect.left();
            int y = j + rect.top();
            QPolygonF polygon = _polygon.translated(pos());
            if (polygon.containsPoint(QPointF(x, y), Qt::OddEvenFill))
            {
                if (x >= 0 && x < image->width() && y >= 0 && y < image->height())
                {
                    float pixelValue = image->getValue(QPoint(x, y));
                    sum += pixelValue;
                    sumOfSquares += pixelValue * pixelValue;
                    if (pixelValue > maxValue)
                    {
                        maxValue = pixelValue;
                    }
                    if (pixelValue < minValue)
                    {
                        minValue = pixelValue;
                    }
                    vecValue.append(pixelValue);
                }
                totalPixelCount++;
            }
        }
    }

    switch (_diagramType)
    {
    case Rect:
        totalPixelCount = qRound(qMax(rect.width() - 2, 0.0) * qMax(rect.height() - 2, 0.0));
        break;
    case Circle:
    case Ellipse:
        totalPixelCount = qRound(M_PI * qMax(rect.width() - 2, 0.0) * qMax(rect.height() - 2, 0.0) / 4);
        break;
    case Rhombus:
        totalPixelCount = qRound(qMax(rect.width() - 2, 0.0) * qMax(rect.height() - 2, 0.0) / 2);
        break;
    default:
        break;
    }

    if (image->hasPixelSpacing())
    {
        _info = QString(tr("Area=%1 mm")).arg(totalPixelCount * image->horzPixelSpacing() * image->vertPixelSpacing());
    }
    else
    {
        _info = QString(tr("Area=%1 px")).arg(totalPixelCount);
    }
    _info += QString(QChar(0x00B2));    // 0x00B2:superscript 2

    if (vecValue.size() > 1)
    {
        float average = sum / vecValue.size();
        float sumSquares = 0;
        for (int i = 0; i < vecValue.size(); i++)
        {
            sumSquares += (vecValue[i] - average) * (vecValue[i] - average);
        }
        float std = sqrt(sumSquares / vecValue.size());

        int decimal = 4;
        if (abs(average) > 10.0f)
        {
            decimal = 1;
        }
        else if (abs(average) > 1.0f)
        {
            decimal = 2;
        }
        else if (abs(average) > 0.1f)
        {
            decimal = 3;
        }

        _info += QString("\n");
        _info += QString(tr("Mean=%1 SD=%2")).arg(QString::number(average, 'f', decimal + 1)).arg(QString::number(std, 'f', decimal + 1));
        _info += QString("\n");
        _info += QString(tr("Max=%1 Min=%2")).arg(QString::number(maxValue, 'f', decimal)).arg(QString::number(minValue, 'f', decimal));
    }
}

QPixmap DiagramPolygonItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(_polygon);

    return pixmap;
}

QList<QPointF> DiagramPolygonItem::resizeHandlePoints() const
{
    qreal width = resizePointWidth;
    QRectF rf = QRectF(boundingRect().topLeft() + QPointF(width / 2, width / 2), boundingRect().bottomRight() - QPointF(width / 2, width / 2));
    qreal centerX = rf.center().x();
    qreal centerY = rf.center().y();
    if (_diagramType == Circle || _diagramType == Ellipse || _diagramType == Rhombus)
    {
        return QList<QPointF>{QPointF(centerX, rf.top()), QPointF(rf.left(), centerY),
            QPointF(rf.right(), centerY), QPointF(centerX, rf.bottom())};
    }
    else if (_diagramType == Parallelogram)
    {
        return QList<QPointF>{rf.topLeft(), QPointF(centerX, rf.top()), QPointF(centerX, rf.bottom()), rf.bottomRight()};
    }
    else
    {
        return QList<QPointF>{rf.topLeft(), QPointF(centerX, rf.top()), rf.topRight(),
            QPointF(rf.left(), centerY), QPointF(rf.right(), centerY),
            rf.bottomLeft(), QPointF(centerX, rf.bottom()), rf.bottomRight()};
    }
}

DiagramPolygonItem* DiagramPolygonItem::clone()
{
    DiagramPolygonItem* cloned = new DiagramPolygonItem(_diagramType, _contextMenu, nullptr);
    cloned->_polygon = _polygon;
    cloned->setPos(scenePos());
    cloned->setPolygon(_polygon);
    cloned->setPen(pen());
    cloned->setBrush(brush());
    cloned->setZValue(zValue());
    cloned->setTransparency(transparency());
    return cloned;
}

void DiagramPolygonItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    GraphicsScene* scene = qobject_cast<GraphicsScene*>(this->scene());
    if (scene->mode() != MOVE_ITEM && scene->mode() != MOVE_ITEM_TEMP)
        return;

    bool resizeMode = false;
    int index = 0;
    foreach(const QPointF& p, resizeHandlePoints())
    {
        if (isCloseEnough(event->pos(), p))
        {
            resizeMode = true;
            break;
        }
        index++;
    }

    index = changeIndex(index);

    _scaleDirection = static_cast<Direction>(index);

    setFlag(GraphicsItemFlag::ItemIsMovable, !resizeMode);
    if (!resizeMode)
    {
        QGraphicsPolygonItem::mousePressEvent(event);
    }
}

void DiagramPolygonItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    GraphicsScene* scene = qobject_cast<GraphicsScene*>(this->scene());
    if (scene->mode() != MOVE_ITEM && scene->mode() != MOVE_ITEM_TEMP)
        return;

    if (_scaleDirection != NoneDirection)
    {
        prepareGeometryChange();
        _polygon = scaledPolygon(_polygon, _scaleDirection, event->pos());
        setPolygon(_polygon);
    }

    statisticsInfo();

    QGraphicsPolygonItem::mouseMoveEvent(event);
}

void DiagramPolygonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    emit itemChanged();

    GraphicsScene* scene = qobject_cast<GraphicsScene*>(this->scene());
    if (scene->mode() != MOVE_ITEM && scene->mode() != MOVE_ITEM_TEMP)
        return;

    _scaleDirection = NoneDirection;
    statisticsInfo();

    QGraphicsPolygonItem::mouseReleaseEvent(event);
}

void DiagramPolygonItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(Qt::ArrowCursor);
    int index = 0;
    bool closeToHandlerPoint = false;
    QList<QPointF> pointList = resizeHandlePoints();
    foreach(const QPointF& p, pointList)
    {
        if (isCloseEnough(p, event->pos()))
        {
            index = changeIndex(index);
            switch (static_cast<Direction>(index))
            {
            case TopLeft:
            case BottomRight:
                setCursor(Qt::SizeFDiagCursor);
                closeToHandlerPoint = true;
                break;
            case Top:
            case Bottom:
                setCursor(Qt::SizeVerCursor);
                closeToHandlerPoint = true;
                break;
            case TopRight:
            case BottomLeft:
                setCursor(Qt::SizeBDiagCursor);
                closeToHandlerPoint = true;
                break;
            case Left:
            case Right:
                setCursor(Qt::SizeHorCursor);
                closeToHandlerPoint = true;
                break;
            default:
                break;
            }
            break;
        }
        index++;
    }

    if (_drawingFinished)
    {
        GraphicsScene* scene = qobject_cast<GraphicsScene*>(this->scene());
        if (closeToHandlerPoint)
        {
            // Close to handler points
            scene->setMode(MOVE_ITEM_TEMP);
        }
        else
        {
            scene->setMode(_previousMode);
        }
    }

    QGraphicsPolygonItem::hoverMoveEvent(event);
}

void DiagramPolygonItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    GraphicsScene* scene = qobject_cast<GraphicsScene*>(this->scene());
    _previousMode = scene->mode();

    QGraphicsPolygonItem::hoverEnterEvent(event);
}

void DiagramPolygonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    // Restore mode
    GraphicsScene* scene = qobject_cast<GraphicsScene*>(this->scene());
    scene->setMode(_previousMode);

    QGraphicsPolygonItem::hoverLeaveEvent(event);
}

void DiagramPolygonItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    QGraphicsPolygonItem::paint(painter, option, widget);

    // add resize handles
    if (isSelected())
    {
        qreal width = resizePointWidth;
        foreach(const QPointF& point, resizeHandlePoints())
        {
            painter->drawEllipse(QRectF(point.x() - width / 2, point.y() - width / 2, width, width));
        }
    }

    QGraphicsScene* graphicsScene = scene();
    QList<QGraphicsView*> viewList = graphicsScene->views();
    Q_ASSERT(viewList.size() > 0);

    QTransform transform = viewList.first()->transform();
    QTransform transform2;
    transform2.translate(boundingRect().right() + 5, boundingRect().center().y() - 25);

    painter->setWorldTransform(transform.inverted() * transform2, true);
    painter->setFont(QFont("Arial", 10));
    painter->setPen(QPen(Qt::yellow));
    painter->drawText(QRectF(0, 0, 260, 90), Qt::TextWordWrap, _info);
}

void DiagramPolygonItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    scene()->clearSelection();
    setSelected(true);
    _contextMenu->exec(event->screenPos());
}

QVariant DiagramPolygonItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
    {
        emit itemSelectedChange(this, value.toBool());
    }

    return QGraphicsItem::itemChange(change, value);;
}

QPolygonF DiagramPolygonItem::scaledPolygon(const QPolygonF& old, DiagramPolygonItem::Direction direction, const QPointF& newPos)
{
    qreal oldWidth = old.boundingRect().width();
    qreal oldHeight = old.boundingRect().height();
    if (direction == NoneDirection || direction == All)
        return old;

    qreal scaleWidth, scaleHeight;
    QTransform trans;
    QPointF fixPoint;

    switch (direction)
    {
    case TopLeft:
    {
        fixPoint = old.boundingRect().bottomRight();
        scaleWidth = (fixPoint.x() - newPos.x()) / oldWidth;
        scaleHeight = (fixPoint.y() - newPos.y()) / oldHeight;
        break;
    }
    case Top:
    {
        fixPoint = old.boundingRect().bottomLeft();
        scaleWidth = 1;
        scaleHeight = (fixPoint.y() - newPos.y()) / oldHeight;
        break;
    }
    case TopRight:
    {
        fixPoint = old.boundingRect().bottomLeft();
        scaleWidth = (newPos.x() - fixPoint.x()) / oldWidth;
        scaleHeight = (fixPoint.y() - newPos.y()) / oldHeight;
        break;
    }
    case Right:
    {
        fixPoint = old.boundingRect().bottomLeft();
        scaleWidth = (newPos.x() - fixPoint.x()) / oldWidth;
        scaleHeight = 1;
        break;
    }
    case BottomRight:
    {
        fixPoint = old.boundingRect().topLeft();
        scaleWidth = (newPos.x() - fixPoint.x()) / oldWidth;
        scaleHeight = (newPos.y() - fixPoint.y()) / oldHeight;
        break;
    }
    case Bottom:
    {
        fixPoint = old.boundingRect().topLeft();
        scaleWidth = 1;
        scaleHeight = (newPos.y() - fixPoint.y()) / oldHeight;
        break;
    }
    case BottomLeft:
    {
        fixPoint = old.boundingRect().topRight();
        scaleWidth = (fixPoint.x() - newPos.x()) / oldWidth;
        scaleHeight = (newPos.y() - fixPoint.y()) / oldHeight;
        break;
    }
    case Left:
    {
        fixPoint = old.boundingRect().bottomRight();
        scaleWidth = (fixPoint.x() - newPos.x()) / oldWidth;
        scaleHeight = 1;
        break;
    }
    default:
    {
        scaleWidth = 0;
        scaleHeight = 0;
        break;
    }
    }

    if (oldWidth * scaleWidth < 10 || oldHeight * scaleHeight < 10)
        return old;

    trans.translate(fixPoint.x(), fixPoint.y());
    trans.scale(scaleWidth, scaleHeight);
    trans.translate(-fixPoint.x(), -fixPoint.y());
    return trans.map(old);
}

int DiagramPolygonItem::changeIndex(int index) const
{
    int newIndex = index;
    if (_diagramType == Circle || _diagramType == Ellipse || _diagramType == Rhombus)
    {
        if (index == 0)
        {
            newIndex = 1;
        }
        else if (index == 1)
        {
            newIndex = 3;
        }
        else
        {
            newIndex *= 2;
        }
    }
    else if (_diagramType == Parallelogram)
    {
        if (index == 2)
        {
            newIndex = 6;
        }
        else if (index == 3)
        {
            newIndex = 7;
        }
        else if (index == 4)
        {
            newIndex = 8;
        }
    }
    return newIndex;
}

bool DiagramPolygonItem::isCloseEnough(const QPointF& p1, const QPointF& p2) const
{
    qreal delta = sqrtf((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()));
    return delta < closeEnoughDistance;
}
