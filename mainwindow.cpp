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
	pDoc = new Document(this);

	// main area for image display
	imageView = new View(this);
	imageView->setStyleSheet("background-color:black");
	setCentralWidget(imageView);

	initUI();

	// For test
//	pDoc->openFile("D:/Qt/John Wagner/STUDY/IM-0001-0001.dcm");//D:/Qt/Class_3_malocclusion/DICOM/I0.dcm//D:/Qt/series-00000/image-00000.dcm
//	pDoc->openFile("D:/test.png");

//	showMaximized();
//	setWindowState(Qt::WindowMaximized);
}

MainWindow::~MainWindow()
{
	if (pDoc)
	{
		delete pDoc;
	}
	if (imageView)
	{
		delete imageView;
	}
}

void MainWindow::initUI()
{
	createActions();

	// setup toolbar
	createToolbar();

	createStatusBar();

	createToolWidget();
}

void MainWindow::createActions()
{
	// create actions, add them to menus
	_openAction = new QAction(tr("&Open..."), this);
	_openDicomAction = new QAction(tr("Open &DICOM file..."), this);
	_openRawAction = new QAction(tr("Open &Raw file..."), this);
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
	_fileMenu->addAction(_openDicomAction);
	_fileMenu->addAction(_openRawAction);
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
	connect(_openDicomAction, &QAction::triggered, this, &MainWindow::openDicomImage);
	connect(_openRawAction, &QAction::triggered, this, &MainWindow::openRawImage);
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
	setIconSize(QSize(32, 32));

	_fileToolBar = addToolBar("File");

	_openToolButton = new QToolButton;
	_openToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	QMenu* menu = new QMenu(this);
	menu->addAction(_openDicomAction);
	menu->addAction(_openRawAction);
	_openToolButton->setMenu(menu);
	_openToolButton->setIcon(QIcon("Resources/svg/open.svg"));
	connect(_openToolButton, &QToolButton::clicked, _openAction, &QAction::triggered);

	_saveToolButton = new QToolButton;
	_saveToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_openDicomAction);
	_saveToolButton->setMenu(menu);
	_saveToolButton->setIcon(QIcon("Resources/svg/save.svg"));
	connect(_saveToolButton, &QToolButton::clicked, _saveAsAction, &QAction::triggered);

	_layoutToolButton = new QToolButton;
	_layoutToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_openDicomAction);
	_layoutToolButton->setMenu(menu);
	_layoutToolButton->setIcon(QIcon("Resources/svg/layout.svg"));
	connect(_layoutToolButton, &QToolButton::clicked, _saveAsAction, &QAction::triggered);

	_fileToolBar->addWidget(_openToolButton);
	_fileToolBar->addWidget(_saveToolButton);
	_fileToolBar->addSeparator();
	_fileToolBar->addWidget(_layoutToolButton);

	_viewToolBar = addToolBar("View");
	// add actions to toolbars
	_viewToolBar->addAction(_zoomInAction);
	_viewToolBar->addAction(_zoomOutAction);
}

void MainWindow::createStatusBar()
{
	// setup status bar
	QStatusBar* mainStatusBar = statusBar();

	QLabel* infoLabel = new QLabel(mainStatusBar);
	infoLabel->setMinimumWidth(160);
	infoLabel->setAlignment(Qt::AlignHCenter);
	connect(imageView, &View::showInfo, infoLabel, &QLabel::setText);

	QLabel* coordinateLabel = new QLabel(mainStatusBar);
	coordinateLabel->setMinimumWidth(100);
	coordinateLabel->setAlignment(Qt::AlignHCenter);
	connect(imageView->view(), &GraphicsView::showCoordinate, coordinateLabel, &QLabel::setText);

	QLabel* pixelValueLabel = new QLabel(mainStatusBar);
	pixelValueLabel->setMinimumWidth(120);
	pixelValueLabel->setAlignment(Qt::AlignHCenter);
	connect(imageView->view(), &GraphicsView::showPixelValue, pixelValueLabel, &QLabel::setText);

	QToolButton* fitScreenButton = new QToolButton(mainStatusBar);
	fitScreenButton->setText("FitScreen");
	connect(fitScreenButton, &QToolButton::clicked, imageView, &View::fitScreen);
	QToolButton* normalButton = new QToolButton(mainStatusBar);
	normalButton->setText("Normal");
	connect(normalButton, &QToolButton::clicked, imageView->view(), &GraphicsView::zoomNormal);

	QToolButton* zoomOutButton = new QToolButton(mainStatusBar);
	zoomOutButton->setText("-");
	connect(zoomOutButton, &QToolButton::clicked, imageView->view(), &GraphicsView::zoomOut);
	QToolButton* zoomInButton = new QToolButton(mainStatusBar);
	zoomInButton->setText("+");
	connect(zoomInButton, &QToolButton::clicked, imageView->view(), &GraphicsView::zoomIn);

	_slider = new QSlider(mainStatusBar);
	_slider->setOrientation(Qt::Horizontal);
	_slider->setMinimum(0);
	_slider->setMaximum(300);
	_slider->setSingleStep(5);
	_slider->setValue(150);
	_slider->setMaximumWidth(200);
	_slider->setTickPosition(QSlider::TicksBelow);
	connect(imageView->view(), &GraphicsView::valueChanged, _slider, &QSlider::setValue);
	connect(_slider, &QSlider::valueChanged, imageView->view(), &GraphicsView::setValue);

	mainStatusBar->addPermanentWidget(infoLabel);
	mainStatusBar->addPermanentWidget(coordinateLabel);
	mainStatusBar->addPermanentWidget(pixelValueLabel);
	mainStatusBar->addPermanentWidget(fitScreenButton);
	mainStatusBar->addPermanentWidget(normalButton);
	mainStatusBar->addPermanentWidget(zoomOutButton);
	mainStatusBar->addPermanentWidget(_slider);
	mainStatusBar->addPermanentWidget(zoomInButton);
}

void MainWindow::createToolWidget()
{
	ToolBoxWidget* toolbox = new ToolBoxWidget();
	createDockWidget(toolbox);
	connect(toolbox, &ToolBoxWidget::setSceneMode, imageView, &View::setSceneMode);
	connect(toolbox, &ToolBoxWidget::setItemType, imageView->scene(), &GraphicsScene::setItemType);
	connect(toolbox, &ToolBoxWidget::setLineColor, imageView->scene(), &GraphicsScene::setLineColor);
	connect(toolbox, &ToolBoxWidget::setFillColor, imageView->scene(), &GraphicsScene::setFillColor);
	connect(toolbox, &ToolBoxWidget::setTextColor, imageView->scene(), &GraphicsScene::setTextColor);
	connect(toolbox, &ToolBoxWidget::setTextFont, imageView->scene(), &GraphicsScene::setTextFont);

	connect(imageView->scene(), &GraphicsScene::itemInserted, toolbox, &ToolBoxWidget::itemInserted);
	connect(imageView->scene(), &GraphicsScene::itemSelected, toolbox, &ToolBoxWidget::itemSelected);
	connect(imageView->scene(), &GraphicsScene::textSelected, toolbox, &ToolBoxWidget::textSelected);

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

	WidgetManager::getInstance()->addWidget(widget);
}

void MainWindow::openImage()
{
	QFileDialog dialog(this);
	dialog.setWindowTitle(tr("Open Image"));
	dialog.setFileMode(QFileDialog::ExistingFile);
	QStringList filters;
	filters << tr("DICOM files (*.dcm)")
		<< tr("Image files (*.png *.bmp *.jpg *.tif)")
		<< tr("Any files (*)");
	dialog.setNameFilters(filters);
	if (dialog.exec())
	{
		QStringList filePaths = dialog.selectedFiles();
		pDoc->openFile(filePaths.at(0));
	}
}

void MainWindow::openDicomImage()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open DICOM Image"),
		"/", QStringLiteral("DICOM files (*.dcm)"));
	if (!fileName.isEmpty())
	{
		pDoc->openFile(fileName);
	}
}

void MainWindow::openRawImage()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Raw Image"),
		"/", QStringLiteral("Raw files (*.raw *.dat)"));
	if (!fileName.isEmpty())
	{
		pDoc->openFile(fileName);
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

void MainWindow::zoomIn()
{
	imageView->view()->zoomIn();
}

void MainWindow::zoomOut()
{
	imageView->view()->zoomOut();
}

void MainWindow::saveAs()
{
	if (getGlobalImage() == nullptr)
		return;

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image As ..."),
		"/", tr("JPG image (*.jpg);;PNG image (*.png);;BMP image (*.bmp);;TIFF image (*.tif);;DICOM image (*.dcm)"));
	if (!fileName.isEmpty())
	{
		pDoc->saveAs(fileName);
	}
}

void MainWindow::close()
{
	pDoc->closeFile();

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
	pDoc->openFile(dir.absoluteFilePath(fileNames.at(index - 1)));
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

	pDoc->openFile(dir.absoluteFilePath(fileNames.at(index + 1)));
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
		_openDicomAction->setText(tr("Open &DICOM file..."));
		_openRawAction->setText(tr("Open &Raw file..."));
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