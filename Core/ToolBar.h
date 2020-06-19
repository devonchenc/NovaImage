#pragma once

#include <QToolBar>

QT_BEGIN_NAMESPACE
class QAction;
class QToolButton;
QT_END_NAMESPACE

class ToolButton;
class MouseHandler;

#include "../Widget/DiagramItem.h"

class ToolBar : public QToolBar
{
	Q_OBJECT

public:
	ToolBar(QWidget* parent = nullptr);
	ToolBar(const QString& title, QWidget* parent = nullptr);
	virtual ~ToolBar();

private:
	void createAction();

	void createButton();

protected:
	void changeEvent(QEvent* event) override;

	bool eventFilter(QObject* obj, QEvent* event) override;

public slots:
	void setMeasurementType(DiagramItem::DiagramType type);

private slots:
	void layoutToolButtonClicked();
	void showInfoToolButtonClicked();
	void imageWindowActionTriggered();
	void ROIWindowActionTriggered();
	void imageWindowToolButtonTriggered(QAction* action);
	void zoomButtonClicked();
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
	QAction* _fullScreenAction;

	QAction* _showAnnotationAction;
	QAction* _showCrossAction;
	QAction* _showScaleAction;
	QAction* _showMeasurementAction;

	QAction* _flipHorizontalAction;
	QAction* _flipVerticalAction;

	QAction* _rotate90CW;
	QAction* _rotate90CCW;
	QAction* _rotate180;
	QAction* _resetTransformation;

	QAction* _imageWindowAction;
	QAction* _ROIWidowAction;
	QAction* _restoreWindowAction;
	QAction* _imageNegativeAction;

	QAction* _fitWindowAction;
	QAction* _1xAction;
	QAction* _2xAction;
	QAction* _4xAction;
	QAction* _8xAction;
	QAction* _zoomInAction;
	QAction* _zoomOutAction;

	QAction* _moveAction;
	QAction* _cursorAction;

	QAction* _rulerAction;
	QAction* _arrowAction;
	QAction* _angleAction;
	QAction* _rectAction;
	QAction* _roundrectAction;
	QAction* _circleAction;
	QAction* _ellipseAction;
	QAction* _rhombusAction;
	QAction* _parallelogramAction;
	QAction* _textAction;

	QToolButton* _openToolButton;
	QToolButton* _saveToolButton;
	QToolButton* _layoutToolButton;
	QToolButton* _showInfoToolButton;
	QToolButton* _flipToolButton;
	QToolButton* _rotateToolButton;

	ToolButton* _imageWindowToolButton;
	ToolButton* _zoomButton;
	ToolButton* _cursorButton;
	ToolButton* _measurementButton;
};
