#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QKeyEvent>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTranslator>
#include <QPluginLoader>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QMimeData>

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
#include "../Widget/CommonWidget.h"
#include "../Widget/CurvesWidget.h"
#include "../Widget/LevelsWidget.h"
#include "../Widget/SettingsDialog.h"
#include "../Notification/NotificationLayout.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _doc(new Document(this))
    , _topView(new View(this))
    , _frontalView(new View(this))
    , _profileView(new View(this))
    , _volumeView(new View(this))
    , _translator(nullptr)
{
    // main area for image display
    QWidget* centerWidget = new QWidget;
    setCentralWidget(centerWidget);

    _layoutManager = new LayoutManager(centerWidget);
    _layoutManager->setWidget(_topView, _frontalView, _profileView, _volumeView);
    _layoutManager->oneView();

    initUI();

    loadPlugin();

    loadTranslator();

    setAcceptDrops(true);

    _notification = new NotificationLayout;
    _recentFiles = new RecentFiles(this);
    _recentFiles->attachToMenuAfterItem(_fileMenu, _printAction, SLOT(openFile(const QString&)));
}

MainWindow::~MainWindow()
{
    if (_topView)
    {
        delete _topView;
        _topView = nullptr;
    }
    if (_frontalView)
    {
        delete _frontalView;
        _frontalView = nullptr;
    }
    if (_profileView)
    {
        delete _profileView;
        _profileView = nullptr;
    }
    if (_volumeView)
    {
        delete _volumeView;
        _volumeView = nullptr;
    }
    if (_doc)
    {
        delete _doc;
        _doc = nullptr;
    }
    if (_translator)
    {
        delete _translator;
        _translator = nullptr;
    }
    for (int i = 0; i < _vecPluginTranslator.size(); i++)
    {
        delete _vecPluginTranslator[i];
    }
    _vecPluginTranslator.clear();
    if (_notification)
    {
        delete _notification;
        _notification = nullptr;
    }
    if (_recentFiles)
    {
        delete _recentFiles;
        _recentFiles = nullptr;
    }
}

void MainWindow::initUI()
{
    createActions();

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
    _settingsAction = new QAction(tr("&Preferences..."), this);

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
    connect(languageGroup, SIGNAL(triggered(QAction*)), this, SLOT(slectLanguage(QAction*)));

    _userGuideAction = new QAction(tr("&User's Guide"));
    _userGuideAction->setIcon(QIcon(":/icon/svg/guide.svg"));
    _aboutAction = new QAction(tr("&About"));
    _aboutAction->setIcon(QIcon(":/icon/svg/about.svg"));

    // setup menubar
    _fileMenu = menuBar()->addMenu(tr("&File"));
    _fileMenu->addAction(_openAction);
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
    _viewMenu->addAction(_zoomOutAction);
    _viewMenu->addAction(_zoomInAction);
    _viewMenu->addSeparator();
    _viewMenu->addAction(_prevImageAction);
    _viewMenu->addAction(_nextImageAction);
    _viewMenu->addSeparator();
    QMenu* languageMenu = _viewMenu->addMenu(tr("&Language"));
    languageMenu->addAction(_engAction);
    languageMenu->addAction(_chsAction);

    _helpMenu = menuBar()->addMenu(tr("&Help"));
    _helpMenu->addAction(_userGuideAction);
    _helpMenu->addSeparator();
    _helpMenu->addAction(_aboutAction);

    // connect the signals and slots
    connect(_openAction, &QAction::triggered, this, &MainWindow::openImage);
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

    connect(_userGuideAction, &QAction::triggered, this, &MainWindow::userGuide);
    connect(_aboutAction, &QAction::triggered, this, &MainWindow::about);

    setupShortcuts();
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

    connect(toolbox, &ToolBoxWidget::setLineColor, _topView->scene(), &GraphicsScene::setLineColor);
    connect(toolbox, &ToolBoxWidget::setEnableFillColor, _topView->scene(), &GraphicsScene::enableFillColor);
    connect(toolbox, &ToolBoxWidget::setFillColor, _topView->scene(), &GraphicsScene::setFillColor);
    connect(toolbox, &ToolBoxWidget::setTransparency, _topView->scene(), &GraphicsScene::setTransparency);
    connect(toolbox, &ToolBoxWidget::setTextColor, _topView->scene(), &GraphicsScene::setTextColor);
    connect(toolbox, &ToolBoxWidget::setTextFont, _topView->scene(), &GraphicsScene::setTextFont);

    connect(_topView->scene(), &GraphicsScene::itemInserted, toolbox, &ToolBoxWidget::itemInserted);
    connect(_topView->scene(), &GraphicsScene::itemSelected, toolbox, &ToolBoxWidget::itemSelected);
    connect(_topView->scene(), &GraphicsScene::textSelected, toolbox, &ToolBoxWidget::textSelected);
    connect(this, &MainWindow::setToolBoxVisible, toolbox, &ToolBoxWidget::setWidgetVisible);

    CommonWidget* common = new CommonWidget();
    createDockWidget(common);

    LevelsWidget* levels = new LevelsWidget();
    QDockWidget* levelDockWidget = createDockWidget(levels);

    CurvesWidget* curves = new CurvesWidget();
    QDockWidget* curveDockWidget = createDockWidget(curves);

    tabifyDockWidget(levelDockWidget, curveDockWidget);
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

void MainWindow::openRawImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Raw Image"),
                                                    "/", tr("Raw file (*.raw *.dat)"));
    if (!fileName.isEmpty())
    {
        _doc->openFile(fileName);
    }
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
    }
}

void MainWindow::setToolBoxWidgetVisible(bool line, bool text)
{
    emit setToolBoxVisible(line, text);
}

QVector<QAction*> MainWindow::mouseActionVector()
{
    return _toolBar->actionVector();
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

void MainWindow::oneView()
{
    _layoutManager->oneView();
}

void MainWindow::threeView()
{
    _layoutManager->threeView();
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
    _topView->view()->showAnnotation(show);
}

void MainWindow::showCrossLine(bool show)
{
    _topView->view()->showCrossLine(show);
}

void MainWindow::showScale(bool show)
{
    _topView->view()->showLineScale(show);
}

void MainWindow::showMeasurement(bool show)
{
    _topView->scene()->showMeasurement(show);
}

void MainWindow::zoomIn()
{
    _topView->view()->zoomIn();
}

void MainWindow::zoomOut()
{
    _topView->view()->zoomOut();
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
    GraphicsView* graphicsView = _topView->view();
    QRectF sceneRect(graphicsView->sceneRect());
    QRect source(graphicsView->mapFromScene(sceneRect.topLeft()), graphicsView->mapFromScene(sceneRect.bottomRight()));
    graphicsView->render(&painter, target, source);
}

void MainWindow::setupShortcuts()
{
    _openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    _closeAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    _printAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    _undoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    _redoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    _exitAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));
    _userGuideAction->setShortcut(QKeySequence(Qt::Key_F1));

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
    _translator = new QTranslator;
    _translator->load("novaimage_zh.qm");

    for (int i = 0; i < _vecPlugin.size(); i++)
    {
        QString name = _vecPlugin[i]->name();
        QString qmName = QCoreApplication::applicationDirPath() + "/plugin/" + name + "_zh.qm";
        QTranslator* tran = new QTranslator;
        if (tran->load(qmName))
        {
            _vecPluginTranslator.append(tran);
        }
    }

    QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
    slectLanguage(settings.value("General/language", 0).toInt());
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
            _doc->saveGraphicsItem();
        }
    }
    return true;
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
    if (index == fileNames.size() - 1)
        index = -1;

    _doc->openFile(dir.absoluteFilePath(fileNames.at(index + 1)));
}

void MainWindow::slectLanguage(QAction* action)
{
    if (action == _engAction)
    {
        slectLanguage(0);
    }
    else if(action == _chsAction)
    {
        slectLanguage(1);
    }
}

void MainWindow::slectLanguage(int language)
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

void MainWindow::setting()
{
    SettingsDialog dlg;
    connect(&dlg, SIGNAL(changeLanguage(int)), this, SLOT(slectLanguage(int)));
    dlg.exec();
}

void MainWindow::userGuide()
{

}

void MainWindow::about()
{

}

void MainWindow::openFile(const QString& fileName)
{
    _doc->openFile(fileName);
}

void MainWindow::notify(const QString& title, const QString& message, int type)
{
    _notification->addNotification(title, message, static_cast<NotificationParams::Type>(type));
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        _fileMenu->setTitle(tr("&File"));
        _editMenu->setTitle(tr("&Edit"));
        _viewMenu->setTitle(tr("&View"));
        _helpMenu->setTitle(tr("&Help"));

        _openAction->setText(tr("&Open..."));
        _saveAsAction->setText(tr("&Save As..."));
        _closeAction->setText(tr("&Close"));
        _printAction->setText(tr("&Print..."));
        _exitAction->setText(tr("E&xit"));

        _undoAction->setText(tr("&Undo"));
        _redoAction->setText(tr("&Redo"));
        _restoreAction->setText(tr("R&estore"));
        _settingsAction->setText(tr("&Preferences..."));

        _zoomInAction->setText(tr("Zoom &In"));
        _zoomOutAction->setText(tr("Zoom &Out"));
        _prevImageAction->setText(tr("&Prev Image"));
        _nextImageAction->setText(tr("&Next Image"));
        _userGuideAction->setText(tr("&User's Guide"));
        _aboutAction->setText(tr("&About"));

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
        QString fileName = urls[0].toLocalFile();
        if (Document::findType(fileName) != IMAGE_FORMAT_UNKNOWN)
        {
            event->acceptProposedAction();
        }
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    QString fileName = urls[0].toLocalFile();
    openFile(fileName);
}
