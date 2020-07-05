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
class PluginInterface;
class NotificationLayout;
class RecentFiles;

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

	void dragEnterEvent(QDragEnterEvent* event) override;

	void dropEvent(QDropEvent* event) override;

private:
    void initUI();

	void createToolWidget();

	QDockWidget* createDockWidget(BaseWidget* widget);

	void createToolbar();

    void createActions();

    void setupShortcuts();

	void loadPlugin();

	void loadTranslator();

signals:
	void setToolBoxVisible(bool line, bool text);

public slots:
    void openImage();
	void openDicomImage();
	void openRawImage();
	void saveAs();
	void saveAsRawImage();
	void showMenuBar(bool show);
	void showDockWidget(bool show);
	void fullScreen();
	void showAnnotation(bool show);
	void showCrossLine(bool show);
	void showScale(bool show);
	void showMeasurement(bool show);
    void zoomIn();
    void zoomOut();
	void close();
	void print();

	void prevImage();
	void nextImage();

	void slectLanguage(QAction* action);
	void slectLanguage(int language);

	void setting();
	void userGuide();
	void about();

	void openFile(const QString& fileName);

	void notify(const QString& title, const QString& message, int type);

private:
    ToolBar* _toolBar;

    View* _view;
	Document* _doc;

	QMenu* _fileMenu;
	QMenu* _editMenu;
	QMenu* _viewMenu;
	QMenu* _helpMenu;

	QAction* _openAction;
	QAction* _saveAsAction;
	QAction* _closeAction;
	QAction* _printAction;
	QAction* _exitAction;
	QAction* _undoAction;
	QAction* _redoAction;
	QAction* _restoreAction;
	QAction* _zoomInAction;
	QAction* _zoomOutAction;
	QAction* _prevImageAction;
	QAction* _nextImageAction;
	QAction* _engAction;
	QAction* _chsAction;
	QAction* _settingsAction;
	QAction* _userGuideAction;
	QAction* _aboutAction;

	QTranslator* _translator;
	QVector<QTranslator*> _vecPluginTranslator;

	QVector<PluginInterface*> _vecPlugin;

	QVector<QDockWidget*> _vecDockWidget;
	QDockWidget* _toolboxDockWidget;

	NotificationLayout* _notification;

	RecentFiles* _recentFiles;
};
