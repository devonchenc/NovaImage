#pragma once

#include <QGraphicsScene>

#include "../Diagram/DiagramItem.h"
#include "../Diagram/DiagramTextItem.h"

QT_BEGIN_NAMESPACE
class QMouseEvent;
QT_END_NAMESPACE

class DiagramLineItem;
class DiagramAngleItem;

#define NO_DRAG			0
#define MOVE_ITEM		1
#define MOVE_ITEM_TEMP	2
#define MOVE_SCENE		3
#define INSERT_ITEM		4

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GraphicsScene(QMenu* itemMenu, QObject* parent = nullptr);
    QFont font() const { return _font; }
    QColor textColor() const { return _textColor; }
    QColor itemColor() const { return _fillColor; }
    QColor lineColor() const { return _lineColor; }

    // utilities
    void deleteItems(const QList<QGraphicsItem*>& items);

    void setMode(int mode);

    int mode() { return _mode; }

    void setItemType(DiagramItem::DiagramType type);

    void showMeasurement(bool show);

    void mousePress(const QPointF& point);

    void mouseMove(const QPointF& point);

    void mouseRelease(const QPointF& point);

    bool saveToFile(const QString& fileName);

    bool loadFromFile(const QString& fileName);

public slots:
    void editorLostFocus(DiagramTextItem* item);
    void setLineColor(const QColor& color);
    void enableFillColor(bool enable);
    void setFillColor(const QColor& color);
    void setTransparency(int value);
    void setTextColor(const QColor& color);
    void setTextFont(const QFont& font);

    void itemSelectedChange(QGraphicsItem* item);

    void itemChanged();

signals:
    void itemInserted(QGraphicsItem* item);
    void itemSelected(QGraphicsItem* item);
    void textSelected(QGraphicsItem* item);

protected:
    void keyPressEvent(QKeyEvent* keyEvent) override;

private:
    DiagramItem::DiagramType _itemType;
    QMenu* _itemMenu;
    int _mode;
    QPointF _startPoint;
    bool _enableFill;
    QColor _lineColor;
    QColor _fillColor;
    QColor _textColor;
    QFont _font;

    DiagramLineItem* _currentDrawingLine;

    DiagramAngleItem* _currentDrawingAngle;

    DiagramItem* _currentDrawingItem;
};
