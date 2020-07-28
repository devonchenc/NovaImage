#pragma once

#include <QGraphicsTextItem>
#include <QDomElement>

QT_BEGIN_NAMESPACE
class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QDomDocument;
QT_END_NAMESPACE

class DiagramTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 3 };

    DiagramTextItem(QGraphicsItem* parent = nullptr);

    int type() const override { return Type; }

    bool contentIsUpdated() { return _contentHasChanged; }
    DiagramTextItem* clone();

    virtual QDomElement saveToXML(QDomDocument* doc);

    virtual void loadFromXML(const QDomElement& e);

signals:
    void lostFocus(DiagramTextItem* item);
    void textSelectedChange(QGraphicsItem* item);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QString _contentLastTime;
    QPointF _positionLastTime;
    bool _contentHasChanged = false;
};
