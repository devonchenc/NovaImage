#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QKeyEvent>
#include <QDebug>
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

#include "mainwindow.h"
#include "View.h"
#include "Document.h"
#include "GlobalFunc.h"
#include "GraphicsView.h"
#include "ToolBar.h"
#include "PluginInterface.h"
#include "../Image/BaseImage.h"
#include "../Widget/ToolBoxWidget.h"
#include "../Widget/WidgetManager.h"
#include "../Widget/CommonWidget.h"
#include "../Widget/CurvesWidget.h"
#include "../Widget/LevelsWidget.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, _translator(nullptr)
{
	_doc = new Document(this);

	// main area for image display
	_view = new View(this);
	_view->setStyleSheet("background-color:black");
	setCentralWidget(_view);

	initUI();

	loadPlugin();

	loadTranslator();

	// For test
//	_doc->openFile("D:/Qt/John Wagner/STUDY/IM-0001-0001.dcm");
//	_doc->openFile("D:/test.png");

//	showMaximized();
//	setWindowState(Qt::WindowMaximized);
}

MainWindow::~MainWindow()
{
	if (_view)
	{
		delete _view;
		_view = nullptr;
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
	_saveAsAction = new QAction(tr("&Save as..."), this);
	_closeAction = new QAction(tr("&Close"), this);
	_printAction = new QAction(tr("&Print"), this);
	_exitAction = new QAction(tr("E&xit"), this);

	_zoomInAction = new QAction(tr("Zoom &in"), this);
	_zoomOutAction = new QAction(tr("Zoom &out"), this);
	_prevImageAction = new QAction(tr("&Prev image"), this);
	_nextImageAction = new QAction(tr("&Next image"), this);

	_engAction = new QAction("&English", this);
	_engAction->setCheckable(true);
	_engAction->setChecked(true);
	_chsAction = new QAction(tr("&Chinese"), this);
	_chsAction->setCheckable(true);
	QActionGroup* languageGroup = new QActionGroup(this);
	languageGroup->addAction(_engAction);
	languageGroup->addAction(_chsAction);
	languageGroup->setExclusive(true);
	connect(languageGroup, SIGNAL(triggered(QAction*)), this, SLOT(slectLanguage(QAction*)));

	// setup menubar
	_fileMenu = menuBar()->addMenu(tr("&File"));
	_fileMenu->addAction(_openAction);
	_fileMenu->addAction(_saveAsAction);
	_fileMenu->addAction(_closeAction);
	_fileMenu->addSeparator();
	_fileMenu->addAction(_printAction);
	_fileMenu->addSeparator();
	_fileMenu->addAction(_exitAction);

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

	// connect the signals and slots
	connect(_openAction, &QAction::triggered, this, &MainWindow::openImage);
	connect(_saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);
	connect(_closeAction, &QAction::triggered, this, &MainWindow::close);
	connect(_printAction, &QAction::triggered, this, &MainWindow::print);
	connect(_exitAction, &QAction::triggered, QApplication::instance(), &QCoreApplication::quit);

	connect(_zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);
	connect(_zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);
	connect(_prevImageAction, &QAction::triggered, this, &MainWindow::prevImage);
	connect(_nextImageAction, &QAction::triggered, this, &MainWindow::nextImage);

	setupShortcuts();
}

void MainWindow::createToolbar()
{
	setIconSize(QSize(42, 32));

	_toolBar = new ToolBar(tr("Toolbar"), this);
	addToolBar(_toolBar);
}

void MainWindow::createToolWidget()
{
	ToolBoxWidget* toolbox = new ToolBoxWidget();
	_toolboxDockWidget = createDockWidget(toolbox);

	connect(toolbox, &ToolBoxWidget::setItemType, _toolBar, &ToolBar::setMeasurementType);

	connect(toolbox, &ToolBoxWidget::setLineColor, _view->scene(), &GraphicsScene::setLineColor);
	connect(toolbox, &ToolBoxWidget::setEnableFillColor, _view->scene(), &GraphicsScene::enableFillColor);
	connect(toolbox, &ToolBoxWidget::setFillColor, _view->scene(), &GraphicsScene::setFillColor);
	connect(toolbox, &ToolBoxWidget::setTextColor, _view->scene(), &GraphicsScene::setTextColor);
	connect(toolbox, &ToolBoxWidget::setTextFont, _view->scene(), &GraphicsScene::setTextFont);

	connect(_view->scene(), &GraphicsScene::itemInserted, toolbox, &ToolBoxWidget::itemInserted);
	connect(_view->scene(), &GraphicsScene::itemSelected, toolbox, &ToolBoxWidget::itemSelected);
	connect(_view->scene(), &GraphicsScene::textSelected, toolbox, &ToolBoxWidget::textSelected);
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
	filters << tr("Scan file (*.dr *.ndr)")
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
	}
}

void MainWindow::setToolBoxWidgetVisible(bool line, bool text)
{
	emit setToolBoxVisible(line, text);
}

void MainWindow::showMenuBar(bool show)
{
	menuBar()->setVisible(show);
}

void MainWindow::showDockWidget(bool show)
{
	for (int i = 0; i < _vecDockWidget.size(); i++)
	{
		QDockWidget* dockWidget = _vecDockWidget[i];
		dockWidget->setVisible(show);
	}
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
	_view->view()->showAnnotation(show);
}

void MainWindow::showCrossLine(bool show)
{
	_view->scene()->showCrossLine(show);
}

void MainWindow::showScale(bool show)
{

}

void MainWindow::showMeasurement(bool show)
{
	_view->scene()->showMeasurement(show);
}

void MainWindow::zoomIn()
{
	_view->view()->zoomIn();
}

void MainWindow::zoomOut()
{
	_view->view()->zoomOut();
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
	_doc->closeFile();

	WidgetManager::getInstance()->reset();
}

void MainWindow::print()
{
	QPrinter printer(QPrinter::HighResolution);
	printer.setPageSize(QPrinter::A4);

	QPainter painter(&printer);
	QRectF target(0, 0, printer.width(), printer.height());
	GraphicsView* graphicsView = _view->view();
	QRectF sceneRect(graphicsView->sceneRect());
	QRect source(graphicsView->mapFromScene(sceneRect.topLeft()), graphicsView->mapFromScene(sceneRect.bottomRight()));
	graphicsView->render(&painter, target, source);

	// TODO
//	QPrintPreviewDialog preview(&printer, this);
//	preview.exec();
}

void MainWindow::setupShortcuts()
{
	_openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));

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

				_vecPlugin.append(plugin);
			}
		}
	}
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
	if (action == _chsAction)
	{
		qApp->installTranslator(_translator);

		for (int i = 0 ; i < _vecPluginTranslator.size(); i++)
		{
			qApp->installTranslator(_vecPluginTranslator[i]);
		}
	}
	else
	{
		qApp->removeTranslator(_translator);

		for (int i = 0; i < _vecPluginTranslator.size(); i++)
		{
			qApp->removeTranslator(_vecPluginTranslator[i]);
		}
	}
}

void MainWindow::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		_fileMenu->setTitle(tr("&File"));
		_viewMenu->setTitle(tr("&View"));

		_openAction->setText(tr("&Open..."));
		_saveAsAction->setText(tr("&Save as..."));
		_closeAction->setText(tr("&Close"));
		_exitAction->setText(tr("E&xit"));
		_zoomInAction->setText(tr("Zoom &in"));
		_zoomOutAction->setText(tr("Zoom &out"));
		_prevImageAction->setText(tr("&Prev image"));
		_nextImageAction->setText(tr("&Next image"));
		_chsAction->setText(tr("&Chinese"));
	}

	QMainWindow::changeEvent(event);
}