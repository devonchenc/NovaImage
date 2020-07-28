#include "GraphicsScene.h"

#include <cmath>
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QKeyEvent>
#include <QBrush>
#include <QApplication>
#include <QDomDocument>
#include <QTextStream>
#include <QTextDocument>

#include "GlobalFunc.h"
#include "View.h"
#include "Document.h"
#include "../Diagram/DiagramTextItem.h"
#include "../Diagram/DiagramLineItem.h"
#include "../Diagram/DiagramLengthItem.h"
#include "../Diagram/DiagramArrowItem.h"
#include "../Diagram/DiagramPlotItem.h"
#include "../Diagram/DiagramAngleItem.h"

#define MIN_SIZE		10

GraphicsScene::GraphicsScene(QMenu* itemMenu, QObject* parent)
    : QGraphicsScene(parent)
    , _itemType(DiagramItem::Rect)
    , _itemMenu(itemMenu)
    , _mode(MOVE_ITEM)
    , _enableFill(false)
    , _lineColor(qRgb(0, 255, 55))
    , _fillColor(Qt::white)
    , _textColor(Qt::green)
    , _currentDrawingLine(nullptr)
    , _currentDrawingAngle(nullptr)
    , _currentDrawingItem(nullptr)
{
    _font.setPointSize(24);
}

void GraphicsScene::setLineColor(const QColor& color)
{
    _lineColor = color;
    foreach (QGraphicsItem* p, selectedItems())
    {
        if (p->type() == DiagramItem::Type)
        {
            DiagramItem* item = qgraphicsitem_cast<DiagramItem*>(p);
            QPen pen = item->pen();
            pen.setColor(color);
            item->setPen(pen);
        }
        else if (p->type() == DiagramLineItem::Type)
        {
            DiagramLineItem* item = qgraphicsitem_cast<DiagramLineItem*>(p);
            QPen pen = item->pen();
            pen.setColor(color);
            item->setPen(pen);
        }
        else if (p->type() == DiagramAngleItem::Type)
        {
            DiagramAngleItem* item = qgraphicsitem_cast<DiagramAngleItem*>(p);
            QPen pen = item->pen();
            pen.setColor(color);
            item->setPen(pen);
        }
    }
}

void GraphicsScene::enableFillColor(bool enable)
{
    _enableFill = enable;
    foreach (QGraphicsItem* p, selectedItems())
    {
        if (p->type() == DiagramItem::Type)
        {
            DiagramItem* item = qgraphicsitem_cast<DiagramItem*>(p);
            QBrush brush = item->brush();
            brush.setStyle(_enableFill ? Qt::SolidPattern : Qt::NoBrush);
            item->setBrush(brush);
        }
        else if (p->type() == DiagramLineItem::Type)
        {
            //	DiagramLineItem* item = qgraphicsitem_cast<DiagramLineItem*>(p);
            //	item->setPointPen(QPen(color));
        }
    }
}

void GraphicsScene::setFillColor(const QColor& color)
{
    _fillColor = color;
    foreach (QGraphicsItem* p, selectedItems())
    {
        if (p->type() == DiagramItem::Type)
        {
            DiagramItem* item = qgraphicsitem_cast<DiagramItem*>(p);
            QBrush brush(_fillColor);
            brush.setStyle(_enableFill ? Qt::SolidPattern : Qt::NoBrush);
            item->setBrush(brush);
        }
        else if (p->type() == DiagramLineItem::Type)
        {
            DiagramLineItem* item = qgraphicsitem_cast<DiagramLineItem*>(p);
            item->setEndpointPen(QPen(color));
        }
    }
}

void GraphicsScene::setTextColor(const QColor& color)
{
    _textColor = color;
    foreach (QGraphicsItem* p, selectedItems())
    {
        if (p->type() == DiagramTextItem::Type)
        {
            DiagramTextItem* item = qgraphicsitem_cast<DiagramTextItem*>(p);
            item->setDefaultTextColor(_textColor);
        }
    }
}

void GraphicsScene::setTextFont(const QFont& font)
{
    _font = font;
    foreach(QGraphicsItem* p, selectedItems())
    {
        if (p->type() == DiagramTextItem::Type)
        {
            DiagramTextItem* item = qgraphicsitem_cast<DiagramTextItem*>(p);
            item->setFont(_font);
        }
    }
}

void GraphicsScene::deleteItems(const QList<QGraphicsItem*>& items)
{
    QList<QGraphicsItem*> diagramItems;
    foreach (QGraphicsItem* item, items)
    {
        diagramItems.append(item);
    }

    foreach (QGraphicsItem* item, diagramItems)
    {
        // Delete unfinished line or angle item
        if (item == _currentDrawingLine)
        {
            _currentDrawingLine = nullptr;
        }
        else if (item == _currentDrawingAngle)
        {
            _currentDrawingAngle = nullptr;
        }

        removeItem(item);
        delete item;
    }
    itemChanged();
}

void GraphicsScene::setMode(int mode)
{
    _mode = mode;
}

void GraphicsScene::setItemType(DiagramItem::DiagramType type)
{
    _itemType = type;
}

void GraphicsScene::showMeasurement(bool show)
{
    QList<QGraphicsItem*> itemList = items();
    for (int i = 0; i < itemList.size(); i++)
    {
        if (itemList.at(i) != getGlobalView()->getPixmapItem())
        {
            itemList.at(i)->setVisible(show);
        }
    }
    update();
}

void GraphicsScene::editorLostFocus(DiagramTextItem* item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty())
    {
        removeItem(item);
        item->deleteLater();
    }
}

void GraphicsScene::mousePress(const QPointF& point)
{
    _startPoint = point;

    // Clear all selected items
    clearSelection();

    if (_mode == INSERT_ITEM)
    {
        if (_itemType <= DiagramItem::Parallelogram)
        {
            _currentDrawingItem = new DiagramItem(_itemType, _itemMenu);
            _currentDrawingItem->setRectF(QRectF(_startPoint, _startPoint));

            QBrush brush(_fillColor);
            brush.setStyle(_enableFill ? Qt::SolidPattern : Qt::NoBrush);
            _currentDrawingItem->setBrush(brush);
            _currentDrawingItem->setPen(QPen(_lineColor, 2));
            connect(_currentDrawingItem, &DiagramItem::itemSelectedChange, this, &GraphicsScene::itemSelectedChange);
            connect(_currentDrawingItem, &DiagramItem::itemChanged, this, &GraphicsScene::itemChanged);
            addItem(_currentDrawingItem);
            itemChanged();
            emit itemInserted(_currentDrawingItem);
        }
        else if (_itemType == DiagramItem::Text)
        {
            DiagramTextItem* textItem = new DiagramTextItem;
            textItem->setFont(_font);
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setZValue(1000.0);
            textItem->setDefaultTextColor(_textColor);
            textItem->setPos(_startPoint);
            connect(textItem, &DiagramTextItem::lostFocus, this, &GraphicsScene::editorLostFocus);
            connect(textItem, &DiagramTextItem::textSelectedChange, this, &GraphicsScene::textSelected);
            connect(textItem->document(), &QTextDocument::contentsChanged, this, &GraphicsScene::itemChanged);
            addItem(textItem);
            itemChanged();
            emit itemInserted(textItem);
        }
        else if (_itemType == DiagramItem::Line || _itemType == DiagramItem::Arrow || _itemType == DiagramItem::Plot)
        {
            if (_itemType == DiagramItem::Line)
            {
                _currentDrawingLine = new DiagramLengthItem(QLineF(_startPoint, _startPoint), _itemMenu);
            }
            else if (_itemType == DiagramItem::Arrow)
            {
                _currentDrawingLine = new DiagramArrowItem(QLineF(_startPoint, _startPoint), _itemMenu);
            }
            else if (_itemType == DiagramItem::Plot)
            {
                _currentDrawingLine = new DiagramPlotItem(QLineF(_startPoint, _startPoint), _itemMenu);
            }
            _currentDrawingLine->setPen(QPen(_lineColor, 2));
            _currentDrawingLine->setEndpointPen(QPen(_fillColor));
            connect(_currentDrawingLine, &DiagramLineItem::itemSelectedChange, this, &GraphicsScene::itemSelectedChange);
            connect(_currentDrawingLine, &DiagramLineItem::itemChanged, this, &GraphicsScene::itemChanged);
            addItem(_currentDrawingLine);
            itemChanged();
            emit itemInserted(_currentDrawingLine);
        }
        else if (_itemType == DiagramItem::Angle)
        {
            if (_currentDrawingAngle == nullptr)
            {
                _currentDrawingAngle = new DiagramAngleItem(_startPoint, _itemMenu);
                _currentDrawingAngle->setPen(QPen(_lineColor, 2));
                _currentDrawingAngle->setEndpointPen(QPen(_fillColor));
                _currentDrawingAngle->setCurrentDrawingIndex(DiagramAngleItem::Point2);
                connect(_currentDrawingAngle, &DiagramAngleItem::itemSelectedChange, this, &GraphicsScene::itemSelectedChange);
                connect(_currentDrawingAngle, &DiagramAngleItem::itemChanged, this, &GraphicsScene::itemChanged);
                addItem(_currentDrawingAngle);
                itemChanged();
                emit itemInserted(_currentDrawingAngle);
            }
            else
            {
                _currentDrawingAngle->setCurrentDrawingIndex(DiagramAngleItem::Point3);
                _currentDrawingAngle->setCurrentDrawingPoint(_startPoint);
            }
        }
    }
}

void GraphicsScene::mouseMove(const QPointF& point)
{
    if ((_itemType == DiagramItem::Line || _itemType == DiagramItem::Arrow || _itemType == DiagramItem::Plot) && _currentDrawingLine != nullptr)
    {
        QPointF p1 = _currentDrawingLine->line().p1();
        if (QApplication::keyboardModifiers() == Qt::ShiftModifier)
        {
            QPointF offset = point - p1;
            if (abs(offset.x()) > abs(offset.y()))
            {
                _currentDrawingLine->setLine(QLineF(p1, QPointF(point.x(), p1.y())));
            }
            else
            {
                _currentDrawingLine->setLine(QLineF(p1, QPointF(p1.x(), point.y())));
            }
        }
        else
        {
            _currentDrawingLine->setLine(QLineF(p1, point));
        }
    }
    else if (_itemType == DiagramItem::Angle && _currentDrawingAngle != nullptr)
    {
        _currentDrawingAngle->setCurrentDrawingPoint(point);
    }
    else if (_itemType <= DiagramItem::Parallelogram)
    {
        if (_currentDrawingItem)
        {
            _currentDrawingItem->setRectF(QRectF(_startPoint, point));
            _currentDrawingItem->statisticsInfo();
        }
    }

    update();
}

void GraphicsScene::mouseRelease(const QPointF& point)
{
    if (_itemType <= DiagramItem::Parallelogram)
    {
        if (_currentDrawingItem)
        {
            QRectF rect(_startPoint, point);
            _currentDrawingItem->setRectF(rect);
            if (fabs(rect.width()) < MIN_SIZE && fabs(rect.height()) < MIN_SIZE)
            {
                removeItem(_currentDrawingItem);
                delete _currentDrawingItem;
            }
            else
            {
                _currentDrawingItem->setDrawingFinished(true);
                _currentDrawingItem->setSelected(true);
            }
            _currentDrawingItem = nullptr;
        }
    }
    else if ((_itemType == DiagramItem::Line || _itemType == DiagramItem::Arrow || _itemType == DiagramItem::Plot) && _currentDrawingLine != nullptr)
    {
        QRectF rect = _currentDrawingLine->boundingRect();
        if (rect.width() < MIN_SIZE && rect.height() < MIN_SIZE)
        {
            removeItem(_currentDrawingLine);
            delete _currentDrawingLine;
        }
        else
        {
            _currentDrawingLine->setDrawingFinished(true);
            _currentDrawingLine->setSelected(true);
        }
        _currentDrawingLine = nullptr;
    }
    else if (_itemType == DiagramItem::Angle && _currentDrawingAngle != nullptr)
    {
        QRectF rect = _currentDrawingAngle->boundingRect();
        if (rect.width() < MIN_SIZE && rect.height() < MIN_SIZE)
        {
            removeItem(_currentDrawingAngle);
            delete _currentDrawingAngle;
            _currentDrawingAngle = nullptr;
        }
        else
        {
            if (_currentDrawingAngle->currentDrawingPoint() == DiagramAngleItem::Point2)
            {
                _currentDrawingAngle->setCurrentDrawingIndex(DiagramAngleItem::Point3);
            }
            else if (_currentDrawingAngle->currentDrawingPoint() == DiagramAngleItem::Point3)
            {
                _currentDrawingAngle->setDrawingFinished(true);
                _currentDrawingAngle->setSelected(true);
                _currentDrawingAngle = nullptr;
            }
        }
    }

    update();
}

bool GraphicsScene::saveToFile(const QString& fileName)
{
    QDomDocument doc("GraphicsScene");
    QDomElement root = doc.createElement("GraphicsItems");
    doc.appendChild(root);

    QList<QGraphicsItem*> itemList = items();
    for (int i = 0; i < itemList.size(); i++)
    {
        switch (itemList.at(i)->type())
        {
        case DiagramItem::Type:
        {
            DiagramItem* item = qgraphicsitem_cast<DiagramItem*>(itemList.at(i));
            QDomElement lineItem = item->saveToXML(&doc);
            root.appendChild(lineItem);
        }
            break;
        case DiagramLineItem::Type:
        {
            DiagramLineItem* item = qgraphicsitem_cast<DiagramLineItem*>(itemList.at(i));
            QDomElement lineItem = item->saveToXML(&doc);
            root.appendChild(lineItem);
        }
            break;
        case DiagramAngleItem::Type:
        {
            DiagramAngleItem* item = qgraphicsitem_cast<DiagramAngleItem*>(itemList.at(i));
            QDomElement lineItem = item->saveToXML(&doc);
            root.appendChild(lineItem);
        }
            break;
        case DiagramTextItem::Type:
        {
            DiagramTextItem* item = qgraphicsitem_cast<DiagramTextItem*>(itemList.at(i));
            QDomElement lineItem = item->saveToXML(&doc);
            root.appendChild(lineItem);
        }
            break;
        default:
            break;
        }
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QTextStream txtOutput(&file);
    txtOutput.setCodec("UTF-8");
    txtOutput << doc.toString();
    file.close();
    return true;
}

bool GraphicsScene::loadFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QDomDocument doc;
    if (!doc.setContent(&file))
    {
        file.close();
        return false;
    }
    file.close();

    QDomNodeList domList = doc.elementsByTagName("GraphicsItem");
    for (int i = 0; i < domList.size(); i++)
    {
        QDomElement diagramElem = domList.at(i).toElement();
        QDomElement attribute = diagramElem.firstChild().toElement();
        if (attribute.isNull())
            break;

        QString type = diagramElem.attribute("Type");
        if (type == "DiagramItem")
        {
            DiagramItem* item = new DiagramItem;
            item->loadFromXML(attribute);
            connect(item, &DiagramItem::itemSelectedChange, this, &GraphicsScene::itemSelectedChange);
            connect(item, &DiagramItem::itemChanged, this, &GraphicsScene::itemChanged);
            addItem(item);
        }
        else if (type == "DiagramLineItem")
        {
            QString name = attribute.attribute("Name");
            DiagramLineItem* item = nullptr;
            if (name == "Length")
            {
                item = new DiagramLengthItem;
            }
            else if (name == "Arrow")
            {
                item = new DiagramArrowItem;
            }
            else if (name == "Plot")
            {
                item = new DiagramPlotItem;
            }
            
            item->loadFromXML(attribute);
            connect(item, &DiagramLineItem::itemSelectedChange, this, &GraphicsScene::itemSelectedChange);
            connect(item, &DiagramLineItem::itemChanged, this, &GraphicsScene::itemChanged);
            addItem(item);
        }
        else if (type == "DiagramAngleItem")
        {
            DiagramAngleItem* item = new DiagramAngleItem;
            item->loadFromXML(attribute);
            connect(item, &DiagramAngleItem::itemSelectedChange, this, &GraphicsScene::itemSelectedChange);
            connect(item, &DiagramAngleItem::itemChanged, this, &GraphicsScene::itemChanged);
            addItem(item);
        }
        else if (type == "DiagramTextItem")
        {
            DiagramTextItem* item = new DiagramTextItem;
            item->loadFromXML(attribute);
            connect(item, &DiagramTextItem::lostFocus, this, &GraphicsScene::editorLostFocus);
            connect(item, &DiagramTextItem::textSelectedChange, this, &GraphicsScene::textSelected);
            connect(item->document(), &QTextDocument::contentsChanged, this, &GraphicsScene::itemChanged);
            addItem(item);
        }
    }

    return true;
}

void GraphicsScene::itemSelectedChange(QGraphicsItem* item)
{
    if (item->type() == DiagramItem::Type)
    {
        DiagramItem* diagramItem = qgraphicsitem_cast<DiagramItem*>(item);
        _lineColor = diagramItem->pen().color();
        _fillColor = diagramItem->brush().color();
    }
    else if (item->type() == DiagramLineItem::Type)
    {
        DiagramLineItem* lineItem = qgraphicsitem_cast<DiagramLineItem*>(item);
        _lineColor = lineItem->pen().color();
        _fillColor = lineItem->pointPen().color();
    }
    else if (item->type() == DiagramAngleItem::Type)
    {
        DiagramAngleItem* angleItem = qgraphicsitem_cast<DiagramAngleItem*>(item);
        _lineColor = angleItem->pen().color();
        _fillColor = angleItem->pointPen().color();
    }

    emit itemSelected(item);
}

void GraphicsScene::itemChanged()
{
    getGlobalDocument()->setModified(true);
}

void GraphicsScene::keyPressEvent(QKeyEvent* keyEvent)
{
    if (keyEvent->key() == Qt::Key_Delete)
    {
        foreach(QGraphicsItem* item, selectedItems())
        {
            removeItem(item);
            delete item;
        }
        itemChanged();
        update();
    }

    QGraphicsScene::keyPressEvent(keyEvent);
}
