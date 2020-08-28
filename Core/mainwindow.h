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
class QPrinter;
QT_END_NAMESPACE

class View;
class Document;
class LayoutManager;
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

    View* getAxialView() { return _axialView; }
    View* getCoronalView() { return _coronalView; }
    View* getSagittalView() { return _sagittalView; }

    View* getActiveView() { return _activeView; }
    void setActiveView(View* view);

    bool isViewLinked();

    void imageOpened();

    void setToolBoxWidgetVisible(bool line, bool text);

    QVector<QAction*> mouseActionVector();

protected:
    void changeEvent(QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
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

    bool querySave();

signals:
    void setToolBoxVisible(bool line, bool text);

public slots:
    void openImage();
    void openDicomImage();
    void openRawImage();
    void saveAs();
    void saveAsRawImage();
    void close();
    void print();
    void printPreview(QPrinter* printer);
    void showMenuBar(bool show);
    void showDockWidget(bool show);
    void setting();

    void singleView();
    void threeView();
    void volumeView();
    void fullScreen();
    void showAnnotation(bool show);
    void showCrossLine(bool show);
    void showScale(bool show);
    void showMeasurement(bool show);
    void zoomIn();
    void zoomOut();

    void slectLayout(QAction* action);

    void prevImage();
    void nextImage();

    void slectLanguage(QAction* action);
    void slectLanguage(int language);

    void userGuide();
    void about();

    void openFile(const QString& fileName);

    void notify(const QString& title, const QString& message, int type);

private:
    ToolBar* _toolBar;

    Document* _doc;
    View* _axialView;
    View* _coronalView;
    View* _sagittalView;
    View* _volumeView;

    View* _activeView;

    LayoutManager* _layoutManager;

    QMenu* _fileMenu;
    QMenu* _editMenu;
    QMenu* _viewMenu;
    QMenu* _processingMenu;
    QMenu* _helpMenu;

    QAction* _openAction;
    QAction* _saveAsAction;
    QAction* _closeAction;
    QAction* _printAction;
    QAction* _exitAction;
    QAction* _undoAction;
    QAction* _redoAction;
    QAction* _restoreAction;
    QAction* _settingsAction;
    QAction* _singleViewAction;
    QAction* _threeViewAction;
    QAction* _volumeViewAction;
    QAction* _linkViewAction;
    QAction* _zoomInAction;
    QAction* _zoomOutAction;
    QAction* _prevImageAction;
    QAction* _nextImageAction;
    QAction* _engAction;
    QAction* _chsAction;
    QAction* _thresholdSegmentationAction;
    QAction* _equalizationAction;
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
