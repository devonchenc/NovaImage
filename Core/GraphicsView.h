#pragma once

#include <QGraphicsView>
#include <QWheelEvent>

#define MAX_ZOOM        1200
#define ZOOM_STEP       200

class View;
class MagnifierWidget;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GraphicsView(View* view, QGraphicsScene* scene, QWidget* parent = nullptr);

    QPointF mapImagePointToScene(qreal x, qreal y) const;

    void setZoomValue(int value);

    void setZoomValueOffset(int offset);

    void showAnnotation(bool show);

    void showCrossLine(bool show);

    void showLineScale(bool show);

    void showMagnifier();

public slots:
    void zoomNormal();
    void zoom2x();
    void zoom4x();
    void zoom8x();
    void zoomIn();
    void zoomOut();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void dragEnterEvent(QDragEnterEvent* event) override;

    void drawForeground(QPainter* painter, const QRectF& rect);

    bool eventFilter(QObject* obj, QEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    void applyZoomValue();

    void drawAnnotation();

    void drawCrossLine();

    void drawLineScale();

    float calcScale(float pixelSpacing);

private:
    View* _view;

    int _zoomFactor;
    QString _strCoord;
    QString _strValue;
    bool _showAnnotation;
    bool _showCrossLine;
    bool _showLineScale;
    MagnifierWidget* _magnifier;
};
