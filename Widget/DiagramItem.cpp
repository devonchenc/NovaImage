#include "DiagramItem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsOpacityEffect>
#include <QTextCodec>
#include <QDebug>

#include "../GlobalFunc.h"
#include "../View.h"
#include "../GraphicsView.h"
#include "../GraphicsScene.h"
#include "../Image/BaseImage.h"

DiagramItem::DiagramItem(DiagramType diagramType, QMenu* contextMenu, QGraphicsItem* parent)
	: QGraphicsPolygonItem(parent)
	, _diagramType(diagramType)
	, _contextMenu(contextMenu)
	, _previousMode(MOVE_ITEM)
{
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

void DiagramItem::setDrawingFinished(bool finished)
{
	_drawingFinished = finished;
}

void DiagramItem::statisticsInfo()
{
	float sum = 0;
	uint totalPixelCount = 0;
	uint validPixelCount = 0;
	float minValue = FLT_MAX;
	float maxValue = -FLT_MAX;
	float sumOfSquares = 0;

	BaseImage* image = getGlobalImage();
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
					validPixelCount++;
				}
				totalPixelCount++;
			}
		}
	}

	if (image->hasPixelSpacing())
	{
		_info = QString(tr("Area=%1 mm")).arg(totalPixelCount * image->horzPixelSpacing() * image->vertPixelSpacing());
	}
	else
	{
		_info = QString(tr("Area=%1 px")).arg(totalPixelCount);
	}
	_info += QString(QChar(0x00B2));	// 0x00B2:superscript 2

	if (validPixelCount > 1)
	{
		float average = sum / validPixelCount;
		float std = sqrt((sumOfSquares - (sum * sum) / validPixelCount) / (validPixelCount - 1));
		_info += QString("\n");
		_info += QString(tr("Mean=%1 SD=%2")).arg(QString::number(average, 'f', 2)).arg(QString::number(std, 'f', 2));
		_info += QString("\n");
		_info += QString(tr("Max=%1 Min=%2")).arg(QString::number(maxValue, 'f', 1)).arg(QString::number(minValue, 'f', 1));
	}
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
		QGraphicsPolygonItem::mousePressEvent(event);
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

	statisticsInfo();

	QGraphicsPolygonItem::mouseMoveEvent(event);
}

void DiagramItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	GraphicsScene* scene = dynamic_cast<GraphicsScene*>(this->scene());
	if (scene->mode() != MOVE_ITEM)
		return;

    _resizeMode = false;
	statisticsInfo();

	QGraphicsPolygonItem::mouseReleaseEvent(event);
}

void DiagramItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(Qt::ArrowCursor);
    int index = 0;
	bool closeToHandlerPoint = false;
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
            }
            break;
        }
        index++;
    }

	if (_drawingFinished)
	{
		GraphicsScene* scene = dynamic_cast<GraphicsScene*>(this->scene());
		if (closeToHandlerPoint)
		{
			// Close to handler points
			scene->setMode(MOVE_ITEM);
		}
		else
		{
			scene->setMode(_previousMode);
		}
	}

	QGraphicsPolygonItem::hoverMoveEvent(event);
}

void DiagramItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	GraphicsScene* scene = dynamic_cast<GraphicsScene*>(this->scene());
	_previousMode = scene->mode();

	QGraphicsPolygonItem::hoverEnterEvent(event);
}

void DiagramItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	// Restore mode
	GraphicsScene* scene = dynamic_cast<GraphicsScene*>(this->scene());
	scene->setMode(_previousMode);

	QGraphicsPolygonItem::hoverLeaveEvent(event);
}

void DiagramItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setRenderHint(QPainter::Antialiasing, true);

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

	QTransform transform = getGlobalView()->view()->transform();
	QTransform transform2;
	transform2.translate(boundingRect().right() + 5, boundingRect().center().y() - 10);

	painter->setWorldTransform(transform.inverted() * transform2, true);
	painter->setFont(QFont("Arial", 10));
	painter->setPen(QPen(Qt::yellow));
	painter->drawText(QRectF(0, 0, 200, 50), Qt::TextWordWrap, _info);
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
	qreal delta = std::sqrtf((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()));
	return delta < closeEnoughDistance;
}
