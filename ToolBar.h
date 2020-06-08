#pragma once

#include <QToolBar>

QT_BEGIN_NAMESPACE
class QAction;
class QToolButton;
QT_END_NAMESPACE

class ToolButton;
class MouseHandler;

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

private slots:
	void imageWindowToolButtonClicked();
	void zoomButtonClicked();
	void selectItem();
	void moveScene();
	void cursorButtonTriggered(QAction* action);
	void cursorButtonUnbounded();
	void measurementChanged();
	void measurementButtonTriggered(QAction* action);

private:
	QAction* _openDicomAction;
	QAction* _openRawAction;

	QAction* _showMenuAction;
	QAction* _showDockWidgetAction;
	QAction* _fullScreenAction;

	QAction* _annotationAction;
	QAction* _crossAction;
	QAction* _scaleAction;

	QAction* _restoreImageWindow;
	QAction* _imageNegativeAction;

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
	QAction* _angleAction;
	QAction* _arrowAction;
	QAction* _rectAction;
	QAction* _roundrectAction;
	QAction* _circleAction;
	QAction* _ellipseAction;
	QAction* _rhombusAction;
	QAction* _parallelogramAction;

	QToolButton* _openToolButton;
	QToolButton* _saveToolButton;
	QToolButton* _layoutToolButton;
	QToolButton* _annotationToolButton;
	QToolButton* _flipToolButton;
	QToolButton* _rotateToolButton;

	ToolButton* _imageWindowToolButton;
	ToolButton* _zoomButton;
	ToolButton* _cursorButton;
	ToolButton* _measurementButton;
};
