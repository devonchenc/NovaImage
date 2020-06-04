#include "DiagramItem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsOpacityEffect>
#include <QDebug>

#include "../GraphicsScene.h"

DiagramItem::DiagramItem(DiagramType diagramType, QMenu* contextMenu, QGraphicsItem* parent)
	: QGraphicsPolygonItem(parent)
	, _diagramType(diagramType)
	, _contextMenu(contextMenu)
{
	QPainterPath path;
	switch (_diagramType)
	{
	case Rect:
		_polygon << QPointF(-90, -60) << QPointF(90, -60) << QPointF(90, 60) << QPointF(-90, 60) << QPointF(-90, -60);
		break;
	case RoundRect:
		path.moveTo(85, 0);
		path.arcTo(25, -60, 60, 60, 0, 90);
		path.arcTo(-85, -60, 60, 60, 90, 90);
		path.arcTo(-85, 0, 60, 60, 180, 90);
		path.arcTo(25, 0, 60, 60, 270, 90);
		path.lineTo(85, 0);
		_polygon = path.toFillPolygon();
		break;
	case Circle:
		path.addEllipse(-80, -80, 160, 160);
		_polygon = path.toFillPolygon();
		break;
	case Ellipse:
		path.addEllipse(-90, -60, 180, 120);
		_polygon = path.toFillPolygon();
		break;
	case Rhombus:
		_polygon << QPointF(-100, 0) << QPointF(0, 100) << QPointF(100, 0) << QPointF(0, -100) << QPointF(-100, 0);
		break;
	case Parallelogram:
		_polygon << QPointF(-110, -60) << QPointF(-70, 60) << QPointF(110, 60) << QPointF(70, -60) << QPointF(-110, -60);
		break;
	}
	setPolygon(_polygon);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	setAcceptHoverEvents(true);

	_effect = new QGraphicsOpacityEffect;
	_effect->setOpacity(1.0);
	setGraphicsEffect(_effect);
}

DiagramItem::~DiagramItem()
{
	delete _effect;
}

void DiagramItem::setRectF(const QRectF& rect)
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
	}
	setPolygon(_polygon);
}

QPixmap DiagramItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(_polygon);

    return pixmap;
}

QList<QPointF> DiagramItem::resizeHandlePoints()
{
    qreal width = resizePointWidth;
    QRectF rf = QRectF(boundingRect().topLeft() + QPointF(width/2, width/2), boundingRect().bottomRight() - QPointF(width/2, width/2));
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

DiagramItem* DiagramItem::clone()
{
    DiagramItem* cloned = new DiagramItem(_diagramType, _contextMenu, nullptr);
    cloned->_polygon = _polygon;
    cloned->setPos(scenePos());
    cloned->setPolygon(_polygon);
	cloned->setPen(pen());
	cloned->setBrush(brush());
    cloned->setZValue(zValue());
    return cloned;
}

void DiagramItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	GraphicsScene* scene = dynamic_cast<GraphicsScene*>(this->scene());
	if (scene->mode() != MOVE_ITEM)
		return;

    _resizeMode = false;
    int index = 0;
    foreach (QPointF const& p, resizeHandlePoints())
	{
        if (isCloseEnough(event->pos(), p))
		{
            _resizeMode = true;
            break;
        }
        index++;
	}

	index = changeIndex(index);

    _scaleDirection = static_cast<Direction>(index);

    setFlag(GraphicsItemFlag::ItemIsMovable, !_resizeMode);
    if (!_resizeMode)
	{
        qDebug() << "item type " << this->type() << " start moving from" << scenePos();
        QGraphicsItem::mousePressEvent(event);
    }
}

void DiagramItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	GraphicsScene* scene = dynamic_cast<GraphicsScene*>(this->scene());
	if (scene->mode() != MOVE_ITEM)
		return;

    if (_resizeMode)
	{
        prepareGeometryChange();
        _polygon = scaledPolygon(_polygon, _scaleDirection, event->pos());
        setPolygon(_polygon);
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void DiagramItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	GraphicsScene* scene = dynamic_cast<GraphicsScene*>(this->scene());
	if (scene->mode() != MOVE_ITEM)
		return;

    _resizeMode = false;
    QGraphicsItem::mouseReleaseEvent(event);
}

void DiagramItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(Qt::ArrowCursor);
    int index = 0;
	QList<QPointF> pointList = resizeHandlePoints();
    foreach (QPointF const& p, pointList)
	{
        if (isCloseEnough(p, event->pos()))
		{
			index = changeIndex(index);
            switch (static_cast<Direction>(index))
			{
            case TopLeft:
            case BottomRight:
				setCursor(Qt::SizeFDiagCursor);
				break;
            case Top:
            case Bottom:
				setCursor(Qt::SizeVerCursor);
				break;
            case TopRight:
            case BottomLeft:
				setCursor(Qt::SizeBDiagCursor);
				break;
            case Left:
            case Right:
				setCursor(Qt::SizeHorCursor);
				break;
            }
            break;
        }
        index++;
    }
    QGraphicsItem::hoverMoveEvent(event);
}

void DiagramItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // remove build-in selected state
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;
    QGraphicsPolygonItem::paint(painter, &myOption, widget);

    // add resize handles
    if (isSelected())
	{
        qreal width = resizePointWidth;
        foreach (QPointF const& point, resizeHandlePoints())
		{
            painter->drawEllipse(QRectF(point.x() - width/2, point.y() - width/2, width, width));
        }
    }
}

void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    scene()->clearSelection();
    setSelected(true);
    _contextMenu->exec(event->screenPos());
}

QVariant DiagramItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (change == QGraphicsItem::ItemSelectedHasChanged)
		emit itemSelectedChange(this);

    return value;
}

QPolygonF DiagramItem::scaledPolygon(const QPolygonF& old, DiagramItem::Direction direction, const QPointF& newPos)
{
	qreal oldWidth = old.boundingRect().width();
	qreal oldHeight = old.boundingRect().height();

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
	}

	if (oldWidth * scaleWidth < 10 || oldHeight * scaleHeight < 10)
		return old;

	trans.translate(fixPoint.x(), fixPoint.y());
	trans.scale(scaleWidth, scaleHeight);
	trans.translate(-fixPoint.x(), -fixPoint.y());
	return trans.map(old);
}

int DiagramItem::changeIndex(int index)
{
	int newIndex = index;
	if (_diagramType == Circle || _diagramType == Ellipse || _diagramType == Rhombus)
	{
		if (index == 0)
			newIndex = 1;
		else if (index == 1)
			newIndex = 3;
		else
			newIndex *= 2;
	}
	else if (_diagramType == Parallelogram)
	{
		if (index == 2)
			newIndex = 6;
		else if (index == 3)
			newIndex = 7;
	}
	return newIndex;
}

bool DiagramItem::isCloseEnough(QPointF const& p1, QPointF const& p2)
{
	qreal delta = std::abs(p1.x() - p2.x()) + std::abs(p1.y() - p2.y());
	return delta < closeEnoughDistance;
}
