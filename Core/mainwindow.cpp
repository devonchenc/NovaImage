#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QDockWidget>
#include <QMenuBar>
#include <QTranslator>
#include <QPluginLoader>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QMimeData>
#include <QTimer>
#include <QCloseEvent>
#include <QDebug>

#include "mainwindow.h"
#include "View.h"
#include "Document.h"
#include "LayoutManager.h"
#include "GlobalFunc.h"
#include "GraphicsView.h"
#include "ToolBar.h"
#include "PluginInterface.h"
#include "RecentFiles.h"
#include "ToolButton.h"
#include "../Image/BaseImage.h"
#include "../Widget/ToolBoxWidget.h"
#include "../Widget/WidgetManager.h"
#include "../Widget/CurvesWidget.h"
#include "../Widget/LevelsWidget.h"
#include "../Widget/ProcessorShellWidget.h"
#include "../Dialog/SettingsDialog.h"
#include "../Dialog/CalibrationDialog.h"
#include "../Dialog/AboutDialog.h"
#include "../Notification/NotificationLayout.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _doc(new Document(this))
    , _axialView(new View(this))
    , _coronalView(new View(this))
    , _sagittalView(new View(this))
    , _volumeView(new View(this))
    , _activeView(_axialView)
    , _translator(nullptr)
    , _timer(this)
{
    // main area for image display
    QWidget* centerWidget = new QWidget;
    setCentralWidget(centerWidget);

    _layoutManager = new LayoutManager(centerWidget);
    _layoutManager->setWidget(_axialView, _coronalView, _sagittalView, _volumeView);
    _layoutManager->singleView(0);

    _coronalView->setViewType(CORONAL_VIEW);
    _sagittalView->setViewType(SAGITTAL_VIEW);
    _volumeView->setViewType(VOLUME_VIEW);

    initUI();

    loadPlugin();

    loadTranslator();

    setAcceptDrops(true);

    _notification = new NotificationLayout(this);
    _recentFiles = new RecentFiles(this);
    _recentFiles->attachToMenuAfterItem(_fileMenu, _printAction, SLOT(openFile(const QString&)));
}

void MainWindow::setActiveView(View* view)
{
    _activeView = view;
    if (getGlobalImage())
    {
        getGlobalImage()->setViewType(view->viewType());
    }
}

bool MainWindow::isViewLinked() const
{
    return _linkViewAction->isChecked();
}

void MainWindow::imageOpened()
{
    if (getGlobalImage())
    {
        WidgetManager::getInstance()->init();

        QString title = QString("%1 - NovaImage").arg(getGlobalImage()->getPathName());
        setWindowTitle(title);

        _recentFiles->setMostRecentFile(getGlobalImage()->getPathName());

        _toolBar->enableButton(true);

        if (getGlobalImage()->slice() > 1)
        {
            _threeViewAction->setEnabled(true);
            _fourViewAction->setEnabled(true);
            _volumeViewAction->setEnabled(true);

            _toolBar->enableViewAction(true);
        }
        else
        {
            _threeViewAction->setEnabled(false);
            _fourViewAction->setEnabled(false);
            _volumeViewAction->setEnabled(false);

            _toolBar->enableViewAction(false);
        }
    }
}

void MainWindow::setToolBoxWidgetVisible(bool line, bool text)
{
    emit setToolBoxVisible(line, text);
}

const QVector<QAction*>& MainWindow::mouseActionVector() const
{
    return _toolBar->actionVector();
}

void MainWindow::levelsAdjust(float bottom, float top)
{
    _levelsWidget->setParameters(bottom, 1.0f, top);
    _levelsWidget->levelsAdjust(bottom, 1.0f, top);
}

LevelsProcessor* MainWindow::getLevelsProcessor() const
{
    return _levelsWidget->getProcessor();
}

bool MainWindow::cine30FPS()
{
    _timer.disconnect();

    if (_timer.isActive() && _timer.interval() == 33)
    {
        _timer.stop();
        return false;
    }
    else
    {
        connect(&_timer, &QTimer::timeout, _activeView, &View::slicePlusOne);
        _timer.start(33);
        return true;
    }
}

bool MainWindow::cine60FPS()
{
    _timer.disconnect();

    if (_timer.isActive() && _timer.interval() == 17)
    {
        _timer.stop();
        return false;
    }
    else
    {
        connect(&_timer, &QTimer::timeout, _activeView, &View::slicePlusOne);
        _timer.start(17);
        return true;
    }
}

void MainWindow::setAxesLocation(QPoint point)
{
    BaseImage* image = getGlobalImage();
    if (image == nullptr)
        return;

    if (_activeView == _axialView)
    {
        image->setSlice(CORONAL_VIEW, point.y());
        image->setSlice(SAGITTAL_VIEW, point.x());
    }
    else if (_activeView == _coronalView)
    {
        image->setSlice(AXIAL_VIEW, point.y());
        image->setSlice(SAGITTAL_VIEW, point.x());
    }
    else if (_activeView == _sagittalView)
    {
        image->setSlice(AXIAL_VIEW, point.y());
        image->setSlice(CORONAL_VIEW, point.x());
    }
    getGlobalDocument()->applyImageWidthAndLevel();
}

void MainWindow::initUI()
{
    createActions();

    createLUTActions();

    setupShortcuts();

    // setup toolbar
    createToolbar();

    createToolWidget();

    setContextMenuPolicy(Qt::NoContextMenu);
}

void MainWindow::createActions()
{
    // create actions, add them to menus
    _openAction = new QAction(tr("&Open..."), this);
    _openAction->setIcon(QIcon(":/icon/svg/open.svg"));
    _openDicomFolderAction = new QAction(tr("Open DICOM &Folder..."), this);
    _saveAsAction = new QAction(tr("&Save As..."), this);
    _saveAsAction->setIcon(QIcon(":/icon/svg/save.svg"));
    _closeAction = new QAction(tr("&Close"), this);
    _printAction = new QAction(tr("&Print..."), this);
    _printAction->setIcon(QIcon(":/icon/svg/print.svg"));
    _exitAction = new QAction(tr("E&xit"), this);

    _undoAction = new QAction(tr("&Undo"), this);
    _undoAction->setIcon(QIcon(":/icon/svg/undo.svg"));
    _redoAction = new QAction(tr("&Redo"), this);
    _redoAction->setIcon(QIcon(":/icon/svg/redo.svg"));
    _restoreAction = new QAction(tr("R&estore"), this);
    _restoreAction->setIcon(QIcon(":/icon/svg/restore.svg"));
    _settingsAction = new QAction(tr("&Settings..."), this);
    _settingsAction->setIcon(QIcon(":/icon/svg/settings.svg"));

    _singleViewAction = new QAction(tr("Single View"), this);
    _singleViewAction->setIcon(QIcon(":/icon/svg/singleview.svg"));
    _singleViewAction->setCheckable(true);
    _singleViewAction->setChecked(true);
    _threeViewAction = new QAction(tr("Three View"), this);
    _threeViewAction->setIcon(QIcon(":/icon/svg/threeview.svg"));
    _threeViewAction->setCheckable(true);
    _threeViewAction->setChecked(false);
    _fourViewAction = new QAction(tr("Four View"), this);
    _fourViewAction->setIcon(QIcon(":/icon/svg/fourview.svg"));
    _fourViewAction->setCheckable(true);
    _fourViewAction->setChecked(false);
    _volumeViewAction = new QAction(tr("Volume View"), this);
    _volumeViewAction->setIcon(QIcon(":/icon/svg/volumeview.svg"));
    _volumeViewAction->setCheckable(true);
    _volumeViewAction->setChecked(false);
    _linkViewAction = new QAction(tr("Link View"), this);
    _linkViewAction->setIcon(QIcon(":/icon/svg/link.svg"));
    _linkViewAction->setCheckable(true);
    _linkViewAction->setChecked(true);

    QActionGroup* viewGroup = new QActionGroup(this);
    viewGroup->addAction(_singleViewAction);
    viewGroup->addAction(_threeViewAction);
    viewGroup->addAction(_fourViewAction);
    viewGroup->addAction(_volumeViewAction);
    viewGroup->setExclusive(true);
    connect(viewGroup, SIGNAL(triggered(QAction*)), this, SLOT(slectLayout(QAction*)));

    _zoomInAction = new QAction(tr("Zoom &In"), this);
    _zoomInAction->setIcon(QIcon(":/icon/svg/zoomin.svg"));
    _zoomOutAction = new QAction(tr("Zoom &Out"), this);
    _zoomOutAction->setIcon(QIcon(":/icon/svg/zoomout.svg"));
    _prevImageAction = new QAction(tr("&Prev Image"), this);
    _nextImageAction = new QAction(tr("&Next Image"), this);

    _engAction = new QAction("&English", this);
    _engAction->setCheckable(true);
    _engAction->setChecked(true);
    _chsAction = new QAction(QStringLiteral("中文（简体）"), this);
    _chsAction->setCheckable(true);
    QActionGroup* languageGroup = new QActionGroup(this);
    languageGroup->addAction(_engAction);
    languageGroup->addAction(_chsAction);
    languageGroup->setExclusive(true);
    connect(languageGroup, SIGNAL(triggered(QAction*)), this, SLOT(selectLanguage(QAction*)));

    _brightnessAndContrastAction = new QAction(tr("&Brightness/Contrast"), this);
    _gammaTransformationAction = new QAction(tr("&Gamma Transformation"), this);
    _thresholdSegmentationAction = new QAction(tr("&Threshold Segmentation"), this);
    _equalizationAction = new QAction(tr("&Histogram Equalization"), this);
    _enhancementAction = new QAction(tr("Image &Enhancement"), this);
    _lightFieldCorrectionAction = new QAction(tr("&Light Field Correction"), this);

    _calibrationAction = new QAction(tr("&Calibration"));

    _userGuideAction = new QAction(tr("&User's Guide"));
    _userGuideAction->setIcon(QIcon(":/icon/svg/guide.svg"));
    _aboutAction = new QAction(tr("&About"));
    _aboutAction->setIcon(QIcon(":/icon/svg/about.svg"));

    // setup menubar
    _fileMenu = menuBar()->addMenu(tr("&File"));
    _fileMenu->addAction(_openAction);
    _fileMenu->addAction(_openDicomFolderAction);
    _fileMenu->addAction(_saveAsAction);
    _fileMenu->addAction(_closeAction);
    _fileMenu->addSeparator();
    _fileMenu->addAction(_printAction);
    _fileMenu->addSeparator();
    _fileMenu->addAction(_exitAction);
    _editMenu = menuBar()->addMenu(tr("&Edit"));
    _editMenu->addAction(_undoAction);
    _editMenu->addAction(_redoAction);
    _editMenu->addSeparator();
    _editMenu->addAction(_restoreAction);
    _editMenu->addSeparator();
    _editMenu->addAction(_settingsAction);

    _viewMenu = menuBar()->addMenu(tr("&View"));
    _viewMenu->addAction(_singleViewAction);
    _viewMenu->addAction(_threeViewAction);
    _viewMenu->addAction(_fourViewAction);
    _viewMenu->addAction(_volumeViewAction);
    _viewMenu->addAction(_linkViewAction);
    _viewMenu->addSeparator();
    _viewMenu->addAction(_zoomOutAction);
    _viewMenu->addAction(_zoomInAction);
    _viewMenu->addSeparator();
    _viewMenu->addAction(_prevImageAction);
    _viewMenu->addAction(_nextImageAction);
    _viewMenu->addSeparator();
    QMenu* languageMenu = _viewMenu->addMenu(tr("&Language"));
    languageMenu->addAction(_engAction);
    languageMenu->addAction(_chsAction);

    _processingMenu = menuBar()->addMenu(tr("&Image Processing"));
    _processingMenu->addAction(_brightnessAndContrastAction);
    _processingMenu->addSeparator();
    _processingMenu->addAction(_gammaTransformationAction);
    _processingMenu->addSeparator();
    _processingMenu->addAction(_equalizationAction);
    _processingMenu->addSeparator();
    _lookupTableMenu = _processingMenu->addMenu(tr("&Lookup Table"));
    _processingMenu->addSeparator();
    _processingMenu->addAction(_thresholdSegmentationAction);
    _processingMenu->addSeparator();
    _processingMenu->addAction(_enhancementAction);
    _processingMenu->addSeparator();
    _processingMenu->addAction(_lightFieldCorrectionAction);

    _toolMenu = menuBar()->addMenu(tr("&Tool"));
    _toolMenu->addAction(_calibrationAction);

    _helpMenu = menuBar()->addMenu(tr("&Help"));
    _helpMenu->addAction(_userGuideAction);
    _helpMenu->addSeparator();
    _helpMenu->addAction(_aboutAction);

    // connect the signals and slots
    connect(_openAction, &QAction::triggered, this, &MainWindow::openImage);
    connect(_openDicomFolderAction, &QAction::triggered, this, &MainWindow::openDicomFolderImage);
    connect(_saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);
    connect(_closeAction, &QAction::triggered, this, &MainWindow::close);
    connect(_printAction, &QAction::triggered, this, &MainWindow::print);
    connect(_exitAction, &QAction::triggered, QApplication::instance(), &QCoreApplication::quit);

    connect(_undoAction, &QAction::triggered, _doc, &Document::undo);
    connect(_redoAction, &QAction::triggered, _doc, &Document::redo);
    connect(_restoreAction, &QAction::triggered, _doc, &Document::restore);
    connect(_settingsAction, &QAction::triggered, this, &MainWindow::setting);

    connect(_zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);
    connect(_zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);
    connect(_prevImageAction, &QAction::triggered, this, &MainWindow::prevImage);
    connect(_nextImageAction, &QAction::triggered, this, &MainWindow::nextImage);

    connect(_gammaTransformationAction, &QAction::triggered, this, &MainWindow::gammaTransformation);
    connect(_brightnessAndContrastAction, &QAction::triggered, this, &MainWindow::brightnessAndContrast);
    connect(_thresholdSegmentationAction, &QAction::triggered, this, &MainWindow::thresholdSegmentation);
    connect(_equalizationAction, &QAction::triggered, this, &MainWindow::equalization);
    connect(_enhancementAction, &QAction::triggered, this, &MainWindow::enhancement);
    connect(_lightFieldCorrectionAction, &QAction::triggered, this, &MainWindow::lightFieldCorrection);

    connect(_calibrationAction, &QAction::triggered, this, &MainWindow::calibration);

    connect(_userGuideAction, &QAction::triggered, this, &MainWindow::userGuide);
    connect(_aboutAction, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createLUTActions()
{
    QDir dir(QCoreApplication::applicationDirPath() + "/LUT");
    if (!dir.exists())
        return;

    QStringList nameFilters;
    nameFilters << "*.lut";
    QStringList fileList = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
    QActionGroup* LUTGroup = new QActionGroup(this);
    for (int i = 0; i < fileList.size(); i++)
    {
        QFileInfo fileInfo(fileList[i]);
        QAction* action = new QAction(fileInfo.completeBaseName(), this);
        action->setCheckable(true);
        LUTGroup->addAction(action);
        _lookupTableMenu->addAction(action);

        connect(action, &QAction::triggered, this, &MainWindow::LUTTriggered);
    }
}

void MainWindow::createToolbar()
{
    setIconSize(QSize(42, 32));

    _toolBar = new ToolBar(tr("Toolbar"), this);
    addToolBar(_toolBar);

    // Load mouse settings
    QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
    QString leftMouseString = settings.value("General/leftMouse", tr("None")).toString();
    QString rightMouseString = settings.value("General/rightMouse", tr("None")).toString();
    QVector<QAction*> vec = mouseActionVector();
    if (leftMouseString != "None")
    {
        for (int i = 0; i < vec.size(); i++)
        {
            if (leftMouseString == vec[i]->objectName())
            {
                QAction* action = vec[i];
                action->trigger();
                ToolButton* toolButton = qobject_cast<ToolButton*>(action->parentWidget());
                toolButton->activeAction(action, true);
                break;
            }
        }
    }
    if (rightMouseString != "None")
    {
        for (int i = 0; i < vec.size(); i++)
        {
            if (rightMouseString == vec[i]->objectName())
            {
                QAction* action = vec[i];
                action->trigger();
                ToolButton* toolButton = qobject_cast<ToolButton*>(action->parentWidget());
                toolButton->activeAction(action, false);
                break;
            }
        }
    }
}

void MainWindow::createToolWidget()
{
    ToolBoxWidget* toolbox = new ToolBoxWidget();
    _toolboxDockWidget = createDockWidget(toolbox);

    connect(toolbox, &ToolBoxWidget::setItemType, _toolBar, &ToolBar::setMeasurementType);

    connect(toolbox, &ToolBoxWidget::setLineColor, this, &MainWindow::setLineColor);
    connect(toolbox, &ToolBoxWidget::setEnableFillColor, this, &MainWindow::enableFillColor);
    connect(toolbox, &ToolBoxWidget::setFillColor, this, &MainWindow::setFillColor);
    connect(toolbox, &ToolBoxWidget::setTransparency, this, &MainWindow::setTransparency);
    connect(toolbox, &ToolBoxWidget::setTextColor, this, &MainWindow::setTextColor);
    connect(toolbox, &ToolBoxWidget::setTextFont, this, &MainWindow::setTextFont);

    connect(_axialView->scene(), &GraphicsScene::itemInserted, toolbox, &ToolBoxWidget::itemInserted);
    connect(_axialView->scene(), &GraphicsScene::itemSelected, toolbox, &ToolBoxWidget::itemSelected);
    connect(_axialView->scene(), &GraphicsScene::textSelected, toolbox, &ToolBoxWidget::textSelected);
    connect(_coronalView->scene(), &GraphicsScene::itemInserted, toolbox, &ToolBoxWidget::itemInserted);
    connect(_coronalView->scene(), &GraphicsScene::itemSelected, toolbox, &ToolBoxWidget::itemSelected);
    connect(_coronalView->scene(), &GraphicsScene::textSelected, toolbox, &ToolBoxWidget::textSelected);
    connect(_sagittalView->scene(), &GraphicsScene::itemInserted, toolbox, &ToolBoxWidget::itemInserted);
    connect(_sagittalView->scene(), &GraphicsScene::itemSelected, toolbox, &ToolBoxWidget::itemSelected);
    connect(_sagittalView->scene(), &GraphicsScene::textSelected, toolbox, &ToolBoxWidget::textSelected);
    connect(this, &MainWindow::setToolBoxVisible, toolbox, &ToolBoxWidget::setWidgetVisible);

    _curvesWidget = new CurvesWidget();
    QDockWidget* curveDockWidget = createDockWidget(_curvesWidget);

    _levelsWidget = new LevelsWidget();
    QDockWidget* levelDockWidget = createDockWidget(_levelsWidget);

    ProcessorShellWidget* processorShellWidget = new ProcessorShellWidget();
    _imageProcessingDockWidget = createDockWidget(processorShellWidget);

    tabifyDockWidget(curveDockWidget, levelDockWidget);
    tabifyDockWidget(levelDockWidget, _imageProcessingDockWidget);

    curveDockWidget->raise();
}

QDockWidget* MainWindow::createDockWidget(BaseWidget* widget)
{
    QDockWidget* dockWidget = new QDockWidget(widget->getName(), this);
    dockWidget->setWidget(widget);
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWidget);
    _vecDockWidget.append(dockWidget);

    WidgetManager::getInstance()->addWidget(widget);

    return dockWidget;
}

void MainWindow::openImage()
{
    QFileDialog dialog(this);
    dialog.setWindowTitle(tr("Open Image"));
    dialog.setFileMode(QFileDialog::ExistingFile);
    QStringList filters;
    filters << tr("All support file (*.dr *.ndr *.dcm *.raw *.dat *.png *.bmp *.jpg *.tif)")
            << tr("Scan file (*.dr *.ndr)")
            << tr("DICOM file (*.dcm)")
            << tr("Raw file (*.raw *.dat)")
            << tr("Image file (*.png *.bmp *.jpg *.tif)")
            << tr("Any file (*)");
    dialog.setNameFilters(filters);
    if (dialog.exec())
    {
        QStringList filePaths = dialog.selectedFiles();
        _doc->openFile(filePaths.at(0));
    }
}

void MainWindow::openDicomImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open DICOM Image"),
                                                    "/", tr("DICOM file (*.dcm)"));
    if (!fileName.isEmpty())
    {
        _doc->openFile(fileName);
    }
}

void MainWindow::openDicomFolderImage()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Open DICOM Folder"), "/");
    if (!path.isEmpty())
    {
        _doc->openFolder(path);
    }
}

void MainWindow::openRawImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Raw Image"),
                                                    "/", tr("Raw file (*.raw *.dat)"));
    if (!fileName.isEmpty())
    {
        _doc->openFile(fileName);
    }
}
void MainWindow::saveAs()
{
    if (getGlobalImage() == nullptr)
        return;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image As ..."),
        "/", tr("JPG image (*.jpg);;PNG image (*.png);;BMP image (*.bmp);;TIFF image (*.tif);;DICOM image (*.dcm);;RAW image (*.raw)"));
    if (!fileName.isEmpty())
    {
        _doc->saveAs(fileName);
    }
}

void MainWindow::saveAsRawImage()
{
    if (getGlobalImage() == nullptr)
        return;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image As ..."),
        "/", tr("RAW image (*.raw)"));
    if (!fileName.isEmpty())
    {
        _doc->saveAs(fileName);
    }
}

void MainWindow::close()
{
    if (!querySave())
        return;

    _doc->closeFile();

    setWindowTitle("NovaImage");

    WidgetManager::getInstance()->reset();

    _toolBar->enableButton(false);
}

void MainWindow::print()
{
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);

    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &MainWindow::printPreview);
    preview.resize(size().height() * 0.8, size().height());
    preview.exec();
}

void MainWindow::printPreview(QPrinter* printer)
{
    QPainter painter(printer);
    QRectF target(0, 0, printer->width(), printer->height());
    GraphicsView* graphicsView = _axialView->view();
    QRectF sceneRect(graphicsView->sceneRect());
    QRect source(graphicsView->mapFromScene(sceneRect.topLeft()), graphicsView->mapFromScene(sceneRect.bottomRight()));
    graphicsView->render(&painter, target, source);
}

void MainWindow::showMenuBar(bool show)
{
    menuBar()->setVisible(show);
}

void MainWindow::showDockWidget(bool show)
{
    for (int i = 0; i < _vecDockWidget.size(); i++)
    {
        _vecDockWidget[i]->setVisible(show);
    }
}

void MainWindow::setting()
{
    SettingsDialog dlg;
    connect(&dlg, SIGNAL(changeLanguage(int)), this, SLOT(selectLanguage(int)));
    dlg.exec();
}

void MainWindow::singleView()
{
    if (_activeView == _axialView)
    {
        _layoutManager->singleView(0);
    }
    else if (_activeView == _coronalView)
    {
        _layoutManager->singleView(1);
    }
    else if (_activeView == _sagittalView)
    {
        _layoutManager->singleView(2);
    }
    else if (_activeView == _volumeView)
    {
        _layoutManager->singleView(3);
    }

    _singleViewAction->setChecked(true);
    _threeViewAction->setChecked(false);
    _fourViewAction->setChecked(false);
    _volumeViewAction->setChecked(false);
    _toolBar->checkViewAction(0);
}

void MainWindow::threeView()
{
    _layoutManager->threeView();

    _singleViewAction->setChecked(false);
    _threeViewAction->setChecked(true);
    _fourViewAction->setChecked(false);
    _volumeViewAction->setChecked(false);
    _toolBar->checkViewAction(1);
}

void MainWindow::fourView()
{
    _layoutManager->fourView();

    _singleViewAction->setChecked(false);
    _threeViewAction->setChecked(false);
    _fourViewAction->setChecked(true);
    _volumeViewAction->setChecked(false);
    _toolBar->checkViewAction(2);
}

void MainWindow::volumeView()
{
    _layoutManager->volumeView();

    _singleViewAction->setChecked(false);
    _threeViewAction->setChecked(false);
    _fourViewAction->setChecked(false);
    _volumeViewAction->setChecked(true);
    _toolBar->checkViewAction(3);
}

void MainWindow::fullScreen()
{
    if (isFullScreen())
    {
        showMaximized();
    }
    else
    {
        showFullScreen();
    }
}

void MainWindow::showAnnotation(bool show)
{
    if (isViewLinked())
    {
        getAxialView()->view()->showAnnotation(show);
        getCoronalView()->view()->showAnnotation(show);
        getSagittalView()->view()->showAnnotation(show);
    }
    else
    {
        getActiveView()->view()->showAnnotation(show);
    }
}

void MainWindow::showCrossLine(bool show)
{
    if (isViewLinked())
    {
        getAxialView()->view()->showCrossLine(show);
        getCoronalView()->view()->showCrossLine(show);
        getSagittalView()->view()->showCrossLine(show);
    }
    else
    {
        getActiveView()->view()->showCrossLine(show);
    }
}

void MainWindow::showThreeViewAxes(bool show)
{
    getAxialView()->view()->showThreeViewAxes(show);
    getCoronalView()->view()->showThreeViewAxes(show);
    getSagittalView()->view()->showThreeViewAxes(show);
}

void MainWindow::showScale(bool show)
{
    if (isViewLinked())
    {
        getAxialView()->view()->showLineScale(show);
        getCoronalView()->view()->showLineScale(show);
        getSagittalView()->view()->showLineScale(show);
    }
    else
    {
        getActiveView()->view()->showLineScale(show);
    }
}

void MainWindow::showMeasurement(bool show)
{
    if (isViewLinked())
    {
        getAxialView()->scene()->showMeasurement(show);
        getCoronalView()->scene()->showMeasurement(show);
        getSagittalView()->scene()->showMeasurement(show);
    }
    else
    {
        getActiveView()->scene()->showMeasurement(show);
    }
}

void MainWindow::zoomIn()
{
    if (isViewLinked())
    {
        getAxialView()->zoomIn();
        getCoronalView()->zoomIn();
        getSagittalView()->zoomIn();
    }
    else
    {
        getActiveView()->zoomIn();
    }
}

void MainWindow::zoomOut()
{
    if (isViewLinked())
    {
        getAxialView()->zoomOut();
        getCoronalView()->zoomOut();
        getSagittalView()->zoomOut();
    }
    else
    {
        getActiveView()->zoomOut();
    }
}

void MainWindow::setZoomValueOffset(int offset)
{
    if (isViewLinked())
    {
        getAxialView()->setZoomValueOffset(offset);
        getCoronalView()->setZoomValueOffset(offset);
        getSagittalView()->setZoomValueOffset(offset);
    }
    else
    {
        getActiveView()->setZoomValueOffset(offset);
    }
}

void MainWindow::setupShortcuts()
{
    _openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    _openDicomFolderAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));
    _closeAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    _printAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    _undoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    _redoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    _exitAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));
    _userGuideAction->setShortcut(QKeySequence(Qt::Key_F1));

    _singleViewAction->setShortcut(QKeySequence(Qt::Key_F2));
    _threeViewAction->setShortcut(QKeySequence(Qt::Key_F3));
    _fourViewAction->setShortcut(QKeySequence(Qt::Key_F4));
    _volumeViewAction->setShortcut(QKeySequence(Qt::Key_F5));

    QList<QKeySequence> shortcuts;
    shortcuts << Qt::Key_Plus << Qt::Key_Equal;
    _zoomInAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Minus << Qt::Key_Underscore;
    _zoomOutAction->setShortcuts(shortcuts);

    _prevImageAction->setShortcut(QKeySequence(Qt::Key_PageUp));
    _nextImageAction->setShortcut(QKeySequence(Qt::Key_PageDown));
}

void MainWindow::loadPlugin()
{
    QDir pluginsDir(QCoreApplication::applicationDirPath() + "/plugin");
    if (!pluginsDir.exists())
        return;

    QStringList nameFilters;
    nameFilters << "*.dll";
    foreach (QString fileName, pluginsDir.entryList(nameFilters, QDir::Files, QDir::Name))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject* instance = loader.instance();
        if (instance)
        {
            PluginInterface* plugin = qobject_cast<PluginInterface*>(instance);
            if (plugin)
            {
                QDockWidget* dockWidget = plugin->createDockWidget(this);
                addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWidget);
                tabifyDockWidget(_toolboxDockWidget, dockWidget);

                _vecDockWidget.append(dockWidget);

                connect(plugin, SIGNAL(openFile(const QString&)), this, SLOT(openFile(const QString&)));
                connect(plugin, SIGNAL(notify(const QString&, const QString&, int)), this, SLOT(notify(const QString&, const QString&, int)));

                _vecPlugin.append(plugin);
            }
        }
    }

    _toolboxDockWidget->raise();
}

void MainWindow::loadTranslator()
{
    _translator = new QTranslator(this);
    _translator->load("novaimage_zh.qm", QCoreApplication::applicationDirPath());

    for (int i = 0; i < _vecPlugin.size(); i++)
    {
        QString name = _vecPlugin[i]->name();
        QString qmName = QCoreApplication::applicationDirPath() + "/plugin/" + name + "_zh.qm";
        QTranslator* tran = new QTranslator(this);
        if (tran->load(qmName))
        {
            _vecPluginTranslator.append(tran);
        }
    }

    QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
    selectLanguage(settings.value("General/language", 0).toInt());
}

bool MainWindow::querySave()
{
    if (_doc->modified())
    {
        QMessageBox::StandardButton reply = QMessageBox::warning(nullptr, tr("Save file"),
            tr("Do you want to save the changes you made to %1?").arg(_doc->getImage()->getPathName()), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel)
        {
            return false;
        }
        else if (reply == QMessageBox::Save)
        {
            _doc->saveGraphicsItems();
        }
    }
    return true;
}

void MainWindow::slectLayout(QAction* action)
{
    if (action == _singleViewAction)
    {
        singleView();
    }
    else if (action == _threeViewAction)
    {
        threeView();
    }
    else if (action == _fourViewAction)
    {
        fourView();
    }
    else if (action == _volumeViewAction)
    {
        volumeView();
    }
}

void MainWindow::prevImage()
{
    BaseImage* image = getGlobalImage();
    if (!image)
        return;

    QString imagePath = image->getPathName();

    QFileInfo current(imagePath);
    QString strTemp = current.fileName();
    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg" << "*.dcm";
    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int index = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if (index == -1)        // not found
        return;

    if (index == 0)
        index = fileNames.size();
    _doc->openFile(dir.absoluteFilePath(fileNames.at(index - 1)));
}

void MainWindow::nextImage()
{
    BaseImage* image = getGlobalImage();
    if (!image)
        return;

    QString imagePath = image->getPathName();

    QFileInfo current(imagePath);
    QString strTemp = current.fileName();
    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg" << "*.dcm";
    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int index = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if (index == -1)        // not found
        return;

    if (index == fileNames.size() - 1)
        index = -1;

    _doc->openFile(dir.absoluteFilePath(fileNames.at(index + 1)));
}

void MainWindow::selectLanguage(QAction* action)
{
    if (action == _engAction)
    {
        selectLanguage(0);
    }
    else if (action == _chsAction)
    {
        selectLanguage(1);
    }
}

void MainWindow::selectLanguage(int language)
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
    settings.setValue("General/language", language);
    if (language == 0)
    {
        qApp->removeTranslator(_translator);

        for (int i = 0; i < _vecPluginTranslator.size(); i++)
        {
            qApp->removeTranslator(_vecPluginTranslator[i]);
        }
        _engAction->setChecked(true);
    }
    else if (language == 1)
    {
        qApp->installTranslator(_translator);

        for (int i = 0; i < _vecPluginTranslator.size(); i++)
        {
            qApp->installTranslator(_vecPluginTranslator[i]);
        }
        _chsAction->setChecked(true);
    }
}

void MainWindow::gammaTransformation()
{
    _imageProcessingDockWidget->raise();
    _doc->gammaTransformation();
}

void MainWindow::brightnessAndContrast()
{
    _imageProcessingDockWidget->raise();
    _doc->brightnessAndContrast();
}

void MainWindow::thresholdSegmentation()
{
    _imageProcessingDockWidget->raise();
    _doc->thresholdSegmentation();
}

void MainWindow::equalization()
{
    _imageProcessingDockWidget->raise();
    _doc->equalization();
}

void MainWindow::LUTTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    _doc->lookupTable(action->text());
}

void MainWindow::enhancement()
{
    _imageProcessingDockWidget->raise();
    _doc->enhancement();
}

void MainWindow::lightFieldCorrection()
{
    _imageProcessingDockWidget->raise();
    _doc->lightFieldCorrection();
}

void MainWindow::calibration()
{
    BaseImage* image = getGlobalImage();
    if (!image)
        return;

    CalibrationDialog* dialog = new CalibrationDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::userGuide()
{

}

void MainWindow::about()
{
    AboutDialog dlg;
    dlg.exec();
}

void MainWindow::setLineColor(const QColor& color)
{
    _activeView->scene()->setLineColor(color);
}

void MainWindow::enableFillColor(bool enable)
{
    _activeView->scene()->enableFillColor(enable);
}

void MainWindow::setFillColor(const QColor& color)
{
    _activeView->scene()->setFillColor(color);
}

void MainWindow::setTransparency(int value)
{
    _activeView->scene()->setTransparency(value);
}

void MainWindow::setTextColor(const QColor& color)
{
    _activeView->scene()->setTextColor(color);
}

void MainWindow::setTextFont(const QFont& font)
{
    _activeView->scene()->setTextFont(font);
}

void MainWindow::openFile(const QString& fileName)
{
    _doc->openFile(fileName);
}

void MainWindow::notify(const QString& title, const QString& message, int type)
{
    _notification->addNotification(title, message, static_cast<NotificationParams::Type>(type));
}

void MainWindow::createProcessorWidget(QWidget* processorWidget)
{
    ProcessorShellWidget* shellWidget = qobject_cast<ProcessorShellWidget*>(_imageProcessingDockWidget->widget());
    if (shellWidget)
    {
        shellWidget->setProcessorWidget(processorWidget);
    }
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        _fileMenu->setTitle(tr("&File"));
        _editMenu->setTitle(tr("&Edit"));
        _viewMenu->setTitle(tr("&View"));
        _processingMenu->setTitle(tr("&Image Processing"));
        _helpMenu->setTitle(tr("&Help"));

        _openAction->setText(tr("&Open..."));
        _openDicomFolderAction->setText(tr("Open DICOM &Folder..."));
        _saveAsAction->setText(tr("&Save As..."));
        _closeAction->setText(tr("&Close"));
        _printAction->setText(tr("&Print..."));
        _exitAction->setText(tr("E&xit"));

        _undoAction->setText(tr("&Undo"));
        _redoAction->setText(tr("&Redo"));
        _restoreAction->setText(tr("R&estore"));
        _settingsAction->setText(tr("&Settings..."));

        _singleViewAction->setText(tr("Single View"));
        _threeViewAction->setText(tr("Three View"));
        _fourViewAction->setText(tr("Four View"));
        _volumeViewAction->setText(tr("Volume View"));
        _linkViewAction->setText(tr("Link View"));
        _zoomInAction->setText(tr("Zoom &In"));
        _zoomOutAction->setText(tr("Zoom &Out"));
        _prevImageAction->setText(tr("&Prev Image"));
        _nextImageAction->setText(tr("&Next Image"));

        _brightnessAndContrastAction->setText(tr("&Brightness/Contrast"));
        _gammaTransformationAction->setText(tr("&Gamma Transformation"));
        _thresholdSegmentationAction->setText(tr("&Threshold Segmentation"));
        _equalizationAction->setText(tr("&Histogram Equalization"));
        _lookupTableMenu->setTitle(tr("&Lookup Table"));
        _enhancementAction->setText(tr("Image &Enhancement"));
        _lightFieldCorrectionAction->setText(tr("&Light Field Correction"));

        _calibrationAction->setText(tr("&Calibration"));

        _userGuideAction->setText(tr("&User's Guide"));
        _aboutAction->setText(tr("&About"));

        _imageProcessingDockWidget->setWindowTitle(tr("Image Processing"));

        _recentFiles->resetText();
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (querySave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        QList<QUrl> urls = event->mimeData()->urls();
        QString url = urls.first().toLocalFile();

        QFileInfo fileInfo(url);
        if (fileInfo.isFile())
        {
            if (Document::findImageType(url) != IMAGE_FORMAT_UNKNOWN)
            {
                event->acceptProposedAction();
            }
        }
        else if (fileInfo.isDir())
        {
            QDir dir(url);
            QStringList nameFilters;
            nameFilters << "*.dcm";
            QStringList fileList = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
            if (fileList.isEmpty() == false)
            {
                event->acceptProposedAction();
            }
        }
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    QString url = urls.first().toLocalFile();

    QFileInfo fileInfo(url);
    if (fileInfo.isFile())
    {
        openFile(url);
    }
    else if (fileInfo.isDir())
    {
        _doc->openFolder(url);
    }
}
