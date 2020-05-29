#pragma once

#include <QToolBar>

QT_BEGIN_NAMESPACE
class QAction;
class QToolButton;
QT_END_NAMESPACE

class ToolBar : public QToolBar
{
	Q_OBJECT

public:
	ToolBar(QWidget* parent = nullptr);
	ToolBar(const QString& title, QWidget* parent = nullptr);
	~ToolBar();

private:
	void createAction();

	void createButton();

	void updateToolButtonIcon();

	QIcon createToolButtonIcon(const QString& imageFile, bool left, bool right);

protected:
	void changeEvent(QEvent* event) override;

	bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
	void selectItem();
	void moveScene();
	void measurementChanged();

private:
	QAction* _openDicomAction;
	QAction* _openRawAction;

	QAction* _showMenuAction;
	QAction* _showDockWidgetAction;
	QAction* _fullScreenAction;

	QAction* _annotationAction;
	QAction* _crossAction;
	QAction* _scaleAction;

	QAction* _imageWindowAction;
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

	QToolButton* _leftMouseButton;
	QToolButton* _rightMouseButton;

	QToolButton* _openToolButton;
	QToolButton* _saveToolButton;
	QToolButton* _layoutToolButton;
	QToolButton* _annotationToolButton;
	QToolButton* _imageWindowToolButton;
	QToolButton* _zoomButton;
	QToolButton* _cursorButton;
	QToolButton* _measurementButton;
};

