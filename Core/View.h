#pragma once

#include <QFrame>
#include <QGraphicsScene>

class DiagramItem;
class GraphicsView;
class PlotDialog;

#include "GraphicsScene.h"

class View : public QFrame
{
    Q_OBJECT

public:
    explicit View(QWidget* parent = nullptr);
    ~View();

    GraphicsView* view() const;

    GraphicsScene* scene();

    int sceneMode();

    void showImage(const QImage* image, bool resetMatrix = false);

    QGraphicsPixmapItem* getPixmapItem() { return _currentImage; }

    void resetImage();

    void setWindowWidthAndLevel(float windowWidth, float windowLevel);

    float windowWidth() { return _windowWidth; }

    float windowLevel() { return _windowLevel; }

    void saveGraphicsItem();

	void loadGraphicsItem();

    void showPlotDialog(QGraphicsLineItem* lineItem);

public slots:
    void setSceneMode(int mode);

    void setItemType(DiagramItem::DiagramType type);

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

    void plotLineWidthChanged(QGraphicsLineItem* lineItem, int lineWidth);

private slots:
    void cutItem();
    void copyItem();
    void pasteItem();
    void deleteItem();

protected:
    void changeEvent(QEvent* event) override;

private:
    void createItemMenus();

    QList<QGraphicsItem*> cloneItems(const QList<QGraphicsItem*>& items);

    void calcPlotData(QGraphicsLineItem* lineItem, int lineWidth);

private:
    GraphicsScene* _scene;
    GraphicsView* _view;

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
};
