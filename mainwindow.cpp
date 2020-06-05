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

#include "mainwindow.h"
#include "View.h"
#include "Document.h"
#include "GlobalFunc.h"
#include "GraphicsView.h"
#include "ToolBar.h"
#include "Image/BaseImage.h"
#include "Widget/ToolBoxWidget.h"
#include "Widget/WidgetManager.h"
#include "Widget/CommonWidget.h"
#include "Widget/CurvesWidget.h"
#include "Widget/LevelsWidget.h"
//#include "Dialog/RawParameter.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	_doc = new Document(this);

	// main area for image display
	_view = new View(this);
	_view->setStyleSheet("background-color:black");
	setCentralWidget(_view);

	initUI();

	// For test
	_doc->openFile("D:/Qt/John Wagner/STUDY/IM-0001-0001.dcm");
//	_doc->openFile("D:/test.png");

//	showMaximized();
//	setWindowState(Qt::WindowMaximized);
}

MainWindow::~MainWindow()
{
	if (_doc)
	{
		delete _doc;
	}
	if (_view)
	{
		delete _view;
	}
}

void MainWindow::initUI()
{
	createActions();

	// setup toolbar
	createToolbar();

	createStatusBar();

	createToolWidget();

	setContextMenuPolicy(Qt::NoContextMenu);
}

void MainWindow::createActions()
{
	// create actions, add them to menus
	_openAction = new QAction(tr("&Open..."), this);
	_saveAsAction = new QAction(tr("&Save as..."), this);
	_closeAction = new QAction(tr("&Close"), this);
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

void MainWindow::createStatusBar()
{
	// setup status bar
	QStatusBar* mainStatusBar = statusBar();

	QLabel* infoLabel = new QLabel(mainStatusBar);
	infoLabel->setMinimumWidth(160);
	infoLabel->setAlignment(Qt::AlignHCenter);
	connect(_view, &View::showInfo, infoLabel, &QLabel::setText);

	QLabel* coordinateLabel = new QLabel(mainStatusBar);
	coordinateLabel->setMinimumWidth(100);
	coordinateLabel->setAlignment(Qt::AlignHCenter);
	connect(_view->view(), &GraphicsView::showCoordinate, coordinateLabel, &QLabel::setText);

	QLabel* pixelValueLabel = new QLabel(mainStatusBar);
	pixelValueLabel->setMinimumWidth(120);
	pixelValueLabel->setAlignment(Qt::AlignHCenter);
	connect(_view->view(), &GraphicsView::showPixelValue, pixelValueLabel, &QLabel::setText);

	QToolButton* fitWindowButton = new QToolButton(mainStatusBar);
	fitWindowButton->setText("FitWindow");
	connect(fitWindowButton, &QToolButton::clicked, _view, &View::fitWindow);
	QToolButton* normalButton = new QToolButton(mainStatusBar);
	normalButton->setText("Normal");
	connect(normalButton, &QToolButton::clicked, _view, &View::zoomNormal);

	QToolButton* zoomOutButton = new QToolButton(mainStatusBar);
	zoomOutButton->setText("-");
	connect(zoomOutButton, &QToolButton::clicked, _view->view(), &GraphicsView::zoomOut);
	QToolButton* zoomInButton = new QToolButton(mainStatusBar);
	zoomInButton->setText("+");
	connect(zoomInButton, &QToolButton::clicked, _view->view(), &GraphicsView::zoomIn);

	_slider = new QSlider(mainStatusBar);
	_slider->setOrientation(Qt::Horizontal);
	_slider->setMinimum(0);
	_slider->setMaximum(MAX_ZOOM);
	_slider->setSingleStep(ZOOM_STEP / 5);
	_slider->setValue(MAX_ZOOM / 2);
	_slider->setMaximumWidth(200);
	_slider->setTickPosition(QSlider::TicksBelow);
	connect(_view->view(), &GraphicsView::zoomValueChanged, _slider, &QSlider::setValue);
	connect(_slider, &QSlider::valueChanged, _view->view(), &GraphicsView::setZoomValue);

	mainStatusBar->addPermanentWidget(infoLabel);
	mainStatusBar->addPermanentWidget(coordinateLabel);
	mainStatusBar->addPermanentWidget(pixelValueLabel);
	mainStatusBar->addPermanentWidget(fitWindowButton);
	mainStatusBar->addPermanentWidget(normalButton);
	mainStatusBar->addPermanentWidget(zoomOutButton);
	mainStatusBar->addPermanentWidget(_slider);
	mainStatusBar->addPermanentWidget(zoomInButton);
}

void MainWindow::createToolWidget()
{
	ToolBoxWidget* toolbox = new ToolBoxWidget();
	createDockWidget(toolbox);
	connect(toolbox, &ToolBoxWidget::setSceneMode, _view, &View::setSceneMode);
	connect(toolbox, &ToolBoxWidget::setItemType, _view, &View::setItemType);
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

	CurvesWidget* curves = new CurvesWidget();
	createDockWidget(curves);

	LevelsWidget* levels = new LevelsWidget();
	createDockWidget(levels);
}

void MainWindow::createDockWidget(BaseWidget* widget)
{
	QDockWidget* dockWidget = new QDockWidget(widget->getName(), this);
	dockWidget->setWidget(widget);
	addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWidget);
	_vecDockWidget.append(dockWidget);

	WidgetManager::getInstance()->addWidget(widget);
}

void MainWindow::openImage()
{
	QFileDialog dialog(this);
	dialog.setWindowTitle(tr("Open Image"));
	dialog.setFileMode(QFileDialog::ExistingFile);
	QStringList filters;
	filters << tr("Image files (*.png *.bmp *.jpg *.tif)")
		<< tr("DICOM files (*.dcm)")
		<< tr("Any files (*)");
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
		"/", QStringLiteral("DICOM files (*.dcm)"));
	if (!fileName.isEmpty())
	{
		_doc->openFile(fileName);
	}
}

void MainWindow::openRawImage()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Raw Image"),
		"/", QStringLiteral("Raw files (*.raw *.dat)"));
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

void MainWindow::showMenuBar()
{
	menuBar()->setVisible(menuBar()->isVisible() == false);
}

void MainWindow::showDockWidget()
{
	for (int i = 0; i < _vecDockWidget.size(); i++)
	{
		QDockWidget* dockWidget = _vecDockWidget[i];
		dockWidget->setVisible(dockWidget->isVisible() == false);
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

void MainWindow::showAnnotation()
{

}

void MainWindow::showCrossLine()
{
	_view->scene()->showCrossLine();
}

void MainWindow::showScale()
{

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
		"/", tr("JPG image (*.jpg);;PNG image (*.png);;BMP image (*.bmp);;TIFF image (*.tif);;DICOM image (*.dcm)"));
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
		_translator = new QTranslator;
		if (_translator->load("novaimage_zh.qm"))
		{
			qApp->installTranslator(_translator);
		}
	}
	else
	{
		qApp->removeTranslator(_translator);
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