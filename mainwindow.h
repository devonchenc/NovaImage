#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLabel;
class QAction;
class QSlider;
class QMenuBar;
class QToolBar;
class QToolButton;
class QGraphicsScene;
class QGraphicsView;
class QGraphicsPixmapItem;
class QTranslator;
QT_END_NAMESPACE

class View;
class Document;
class BaseWidget;
class ToolBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent=nullptr);
    ~MainWindow();

public:
	Document* getDocument() { return _doc; }
	View* getView() { return _view; }

	void imageOpened();

	void setToolBoxWidgetVisible(bool line, bool text);

protected:
	void changeEvent(QEvent* event) override;

private:
    void initUI();

	void createToolWidget();

	QDockWidget* createDockWidget(BaseWidget* widget);

	void createToolbar();

	void createStatusBar();

    void createActions();
    void setupShortcuts();

signals:
	void setToolBoxVisible(bool line, bool text);

public slots:
    void openImage();
	void openDicomImage();
	void openRawImage();
	void saveAs();
	void showMenuBar();
	void showDockWidget();
	void fullScreen();
	void showAnnotation();
	void showCrossLine();
	void showScale();
    void zoomIn();
    void zoomOut();
	void close();
	void print();

	void prevImage();
	void nextImage();

//	void openButtonTriggered();

	void slectLanguage(QAction* action);

private:
    ToolBar* _toolBar;

    View* _view;
	Document* _doc;

	QSlider* _slider;

	QMenu* _fileMenu;
	QMenu* _viewMenu;

	QAction* _openAction;
	QAction* _saveAsAction;
	QAction* _closeAction;
	QAction* _printAction;
	QAction* _exitAction;
	QAction* _zoomInAction;
	QAction* _zoomOutAction;
	QAction* _prevImageAction;
	QAction* _nextImageAction;
	QAction* _engAction;
	QAction* _chsAction;

	QTranslator* _translator;

	QVector<QDockWidget*> _vecDockWidget;
};
