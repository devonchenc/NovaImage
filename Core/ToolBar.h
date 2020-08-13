#pragma once

#include <QToolBar>

QT_BEGIN_NAMESPACE
class QAction;
class QToolButton;
QT_END_NAMESPACE

class ToolButton;
class MouseHandler;

#include "../Diagram/DiagramItem.h"

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    ToolBar(QWidget* parent = nullptr);
    ToolBar(const QString& title, QWidget* parent = nullptr);
    virtual ~ToolBar();

    QVector<QAction*> actionVector() { return _actionVector; }

    void enableButton(bool flag);

    void enableViewAction(bool flag);

private:
    void createButton();

    void createAction();

    void initButton();

protected:
    void changeEvent(QEvent* event) override;

    bool eventFilter(QObject* obj, QEvent* event) override;

public slots:
    void setMeasurementType(DiagramItem::DiagramType type);

private slots:
    void layoutButtonClicked();
    void showInfoButtonClicked();
    void singleViewActionTriggered();
    void threeViewActionTriggered();
    void volumeViewActionTriggered();
    void FPS30ActionTriggered();
    void FPS60ActionTriggered();
    void sliceActionTriggered();
    void imageWindowActionTriggered();
    void ROIWindowActionTriggered();
    void imageWindowButtonTriggered(QAction* action);
    void zoomActionTriggered();
    void magnifierActionTriggered();
    void zoomButtonTriggered(QAction* action);
    void selectItem();
    void moveScene();
    void cursorButtonTriggered(QAction* action);
    void measurementChanged();
    void measurementButtonTriggered(QAction* action);

private:
    QAction* _openDicomAction;
    QAction* _openRawAction;
    QAction* _saveAsRawAction;

    QAction* _showMenuAction;
    QAction* _showDockWidgetAction;
    QAction* _singleViewAction;
    QAction* _threeViewAction;
    QAction* _volumeViewAction;
    QAction* _fullScreenAction;

    QAction* _showAnnotationAction;
    QAction* _showCrossAction;
    QAction* _showScaleAction;
    QAction* _showMeasurementAction;

    QAction* _flipHorizontalAction;
    QAction* _flipVerticalAction;

    QAction* _rotate90CWAction;
    QAction* _rotate90CCWAction;
    QAction* _rotate180Action;
    QAction* _resetTransformation;

    QAction* _FPS30Action;
    QAction* _FPS60Action;

    QAction* _sliceAction;

    QAction* _imageWindowAction;
    QAction* _ROIWidowAction;
    QAction* _defaultWindowAction;
    QAction* _fullWindowAction;
    QAction* _imageNegativeAction;

    QAction* _zoomAction;
    QAction* _magnifierAction;
    QAction* _fitWindowAction;
    QAction* _1xAction;
    QAction* _2xAction;
    QAction* _4xAction;
    QAction* _8xAction;
    QAction* _zoomInAction;
    QAction* _zoomOutAction;

    QAction* _cursorAction;
    QAction* _moveAction;

    QAction* _rulerAction;
    QAction* _arrowAction;
    QAction* _angleAction;
    QAction* _plotAction;
    QAction* _rectAction;
    QAction* _roundrectAction;
    QAction* _circleAction;
    QAction* _ellipseAction;
    QAction* _rhombusAction;
    QAction* _parallelogramAction;
    QAction* _textAction;

    QToolButton* _openButton;
    QToolButton* _saveButton;
    QToolButton* _printButton;
    QToolButton* _layoutButton;
    QToolButton* _showInfoButton;
    QToolButton* _flipButton;
    QToolButton* _rotateButton;
    QToolButton* _cineButton;

    ToolButton* _sliceButton;
    ToolButton* _imageWindowButton;
    ToolButton* _zoomButton;
    ToolButton* _cursorButton;
    ToolButton* _measurementButton;

    QToolButton* _undoButton;
    QToolButton* _restoreButton;

    QVector<QAction*> _actionVector;
};
