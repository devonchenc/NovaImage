#pragma once

#include <QFrame>

class DiagramPolygonItem;
class GraphicsView;
class PlotDialog;
class ImageQualityDialog;

#ifndef VIEW_TYPE
#define AXIAL_VIEW              0
#define CORONAL_VIEW            1
#define SAGITTAL_VIEW           2
#define VOLUME_VIEW             3
#endif

#include "GraphicsScene.h"

class View : public QFrame
{
    Q_OBJECT

public:
    explicit View(QWidget* parent = nullptr);

    GraphicsView* view() const;

    GraphicsScene* scene() const;

    int sceneMode() const;

    void setViewType(int type) { _viewType = type; }

    int viewType() const { return _viewType; }

    void showImage(const QImage* image);
    
    void resetMatrix();

    QGraphicsPixmapItem* getPixmapItem() { return _currentImage; }

    int imageWidth() const;

    int imageHeight() const;

    int imageSlice() const;

    int imageCurrentSlice() const;

    float getImageValue(int x, int y) const;
    float getImageValue(const QPoint& position) const;
    float getImageValue(qreal x, qreal y) const;

    QString imageDescription() const;

    void resetImage();

    void setWindowWidthAndLevel(float windowWidth, float windowLevel);

    float windowWidth() { return _windowWidth; }

    float windowLevel() { return _windowLevel; }

    void slicePlus(int plus);
    void sliceMinus(int minus);

    void saveGraphicsItem(QDomDocument& doc, QDomElement& root);

    void loadGraphicsItem(const QDomElement& sceneElem);

    void showPlotDialog(QGraphicsLineItem* lineItem);

    void showImageQualityDialog(QGraphicsLineItem* lineItem, float leftRatio, float rightRatio);

    void ROIWindow(const QRectF& rect);

    void setSceneMode(int mode);

    void setItemType(DiagramPolygonItem::DiagramType type);

    void flipHorizontal();
    void flipVertical();
    void rotate90CW();
    void rotate90CCW();
    void rotate180();
    void resetTransformation();

    void fitWindow();
    void zoomNormal();
    void zoom2x();
    void zoom4x();
    void zoom8x();
    void zoomIn();
    void zoomOut();
    void setZoomValueOffset(int offset);

public slots:
    void slicePlusOne();
    void sliceMinusOne();

    void plotLineWidthChanged(QGraphicsLineItem* lineItem, int lineWidth);
    void imageQualityLineWidthChanged(QGraphicsLineItem* lineItem, int lineWidth);

private slots:
    void cutItem();
    void copyItem();
    void pasteItem();
    void deleteItem();

protected:
    void changeEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void createItemMenus();

    QList<QGraphicsItem*> cloneItems(const QList<QGraphicsItem*>& items);

    QVector<qreal> calcPlotData(QGraphicsLineItem* lineItem, int lineWidth);

private:
    GraphicsScene* _scene;
    GraphicsView* _view;

    int _viewType;

    QGraphicsPixmapItem* _currentImage;

    QMenu* _itemMenu;
    QAction* _copyAction;
    QAction* _pasteAction;
    QAction* _cutAction;
    QAction* _deleteAction;

    QList<QGraphicsItem*> pasteBoard;

    // Window width (for image display)
    float _windowWidth;
    float _windowLevel;

    PlotDialog* _plotDlg;
    ImageQualityDialog* _imageQualityDlg;
};
