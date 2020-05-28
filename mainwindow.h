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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent=nullptr);
    ~MainWindow();

public:
	Document* getDocument() { return pDoc; }
	View* getView() { return imageView; }

	void imageOpened();

protected:
	void changeEvent(QEvent* event) override;

private:
    void initUI();

	void createToolWidget();

	void createDockWidget(BaseWidget* widget);

	void createToolbar();

	void createStatusBar();

    void createActions();
    void setupShortcuts();

private slots:
    void openImage();
	void openDicomImage();
	void openRawImage();
    void zoomIn();
    void zoomOut();
    void saveAs();
	void close();

	void prevImage();
	void nextImage();

//	void openButtonTriggered();

	void slectLanguage(QAction* action);

private:
    QToolBar* _fileToolBar;
    QToolBar* _viewToolBar;

    View* imageView;
	Document* pDoc;

	QSlider* _slider;

	QMenu* _fileMenu;
	QMenu* _viewMenu;

	QAction* _openAction;
	QAction* _openDicomAction;
	QAction* _openRawAction;
	QAction* _saveAsAction;
	QAction* _closeAction;
	QAction* _exitAction;
    QAction* _zoomInAction;
	QAction* _zoomOutAction;
	QAction* _prevImageAction;
	QAction* _nextImageAction;
	QAction* _engAction;
	QAction* _chsAction;

	QToolButton* _openToolButton;
	QToolButton* _saveToolButton;
	QToolButton* _layoutToolButton;

	QTranslator* _translator;
};
