#pragma once

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
class QAction;
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
class CurvesWidget;
class LevelsWidget;
class LevelsProcessor;
class CalibrationDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() {}

public:
    Document* getDocument() const { return _doc; }

    View* getAxialView() const { return _axialView; }
    View* getCoronalView() const { return _coronalView; }
    View* getSagittalView() const { return _sagittalView; }

    View* getActiveView() const { return _activeView; }
    void setActiveView(View* view);

    bool isViewLinked() const;

    void imageOpened();

    void setToolBoxWidgetVisible(bool line, bool text);

    const QVector<QAction*>& mouseActionVector() const;

    void levelsAdjust(float bottom, float top);

    LevelsProcessor* getLevelsProcessor() const;

    bool cine30FPS();
    bool cine60FPS();

    // Set three view axes location
    void setAxesLocation(QPoint point);

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

    void createLUTActions();

    void setupShortcuts();

    void loadPlugin();

    void loadTranslator();

    bool querySave();

signals:
    void setToolBoxVisible(bool line, bool text);

public slots:
    void openImage();
    void openDicomImage();
    void openDicomFolderImage();
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
    void fourView();
    void volumeView();
    void fullScreen();
    void showAnnotation(bool show);
    void showCrossLine(bool show);
    void showThreeViewAxes(bool show);
    void showScale(bool show);
    void showMeasurement(bool show);
    void zoomIn();
    void zoomOut();
    void setZoomValueOffset(int offset);

    void slectLayout(QAction* action);

    void prevImage();
    void nextImage();

    void selectLanguage(QAction* action);
    void selectLanguage(int language);

    void gammaTransformation();
    void brightnessAndContrast();
    void thresholdSegmentation();
    void equalization();
    void LUTTriggered();
    void enhancement();
    void lightFieldCorrection();

    void calibration();

    void userGuide();
    void about();

    void setLineColor(const QColor& color);
    void enableFillColor(bool enable);
    void setFillColor(const QColor& color);
    void setTransparency(int value);
    void setTextColor(const QColor& color);
    void setTextFont(const QFont& font);

    void openFile(const QString& fileName);

    void notify(const QString& title, const QString& message, int type);

    void createProcessorWidget(QWidget* processorWidget);

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
    QMenu* _lookupTableMenu;
    QMenu* _toolMenu;
    QMenu* _helpMenu;

    QAction* _openAction;
    QAction* _openDicomFolderAction;
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
    QAction* _fourViewAction;
    QAction* _volumeViewAction;
    QAction* _linkViewAction;
    QAction* _zoomInAction;
    QAction* _zoomOutAction;
    QAction* _prevImageAction;
    QAction* _nextImageAction;
    QAction* _engAction;
    QAction* _chsAction;

    QAction* _brightnessAndContrastAction;
    QAction* _gammaTransformationAction;
    QAction* _thresholdSegmentationAction;
    QAction* _equalizationAction;
    QAction* _enhancementAction;
    QAction* _lightFieldCorrectionAction;

    QAction* _calibrationAction;

    QAction* _userGuideAction;
    QAction* _aboutAction;

    QTranslator* _translator;
    QVector<QTranslator*> _vecPluginTranslator;

    QVector<PluginInterface*> _vecPlugin;

    CurvesWidget* _curvesWidget;
    LevelsWidget* _levelsWidget;

    QVector<QDockWidget*> _vecDockWidget;
    QDockWidget* _toolboxDockWidget;
    QDockWidget* _imageProcessingDockWidget;

    CalibrationDialog* _calibrationDialog = nullptr;

    NotificationLayout* _notification;

    RecentFiles* _recentFiles;

    QTimer _timer;
};
