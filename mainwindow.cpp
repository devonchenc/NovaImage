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
	setCentralWidget(imageView);

	initUI();

	// For test
	pDoc->openFile("D://test.png");

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
    // setup toolbar
    fileToolBar = addToolBar("File");
    viewToolBar = addToolBar("View");

	createStatusBar();

	createActions();

	createToolWidget();
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

void MainWindow::createActions()
{
    // create actions, add them to menus
	_openAction = new QAction(tr("&Open..."), this);
	_saveAsAction = new QAction(tr("&Save as..."), this);
	_exitAction = new QAction(tr("E&xit"), this);
    _zoomInAction = new QAction(tr("Zoom &in"), this);
    _zoomOutAction = new QAction(tr("Zoom &out"), this);

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
	_fileMenu->addSeparator();
	_fileMenu->addAction(_exitAction);

	_viewMenu = menuBar()->addMenu(tr("&View"));
	_viewMenu->addAction(_zoomOutAction);
	_viewMenu->addAction(_zoomInAction);
	_viewMenu->addSeparator();
	QMenu* languageMenu = _viewMenu->addMenu(tr("&Language"));
	languageMenu->addAction(_engAction);
	languageMenu->addAction(_chsAction);

    // add actions to toolbars
    fileToolBar->addAction(_openAction);
    viewToolBar->addAction(_zoomInAction);
    viewToolBar->addAction(_zoomOutAction);

    // connect the signals and slots
    connect(_openAction, &QAction::triggered, this, &MainWindow::openImage);
	connect(_saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);
	connect(_exitAction, &QAction::triggered, QApplication::instance(), &QCoreApplication::quit);
    connect(_zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);
    connect(_zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);

    setupShortcuts();
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
		<< tr("Image files (*.png *.bmp *.jpg)")
		<< tr("Any files (*)");
	dialog.setNameFilters(filters);
    if (dialog.exec())
	{
		QStringList filePaths = dialog.selectedFiles();
		pDoc->openFile(filePaths.at(0));

		if (getGlobalImage())
		{
			WidgetManager::getInstance()->init();
		}
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
/*    if (currentImage == nullptr)
	{
        QMessageBox::information(this, "Information", "Nothing to save.");
        return;
    }
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save Image As ...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList fileNames;
    if (dialog.exec())
	{
        fileNames = dialog.selectedFiles();
        if (QRegExp(".+\\.(png|bmp|jpg)").exactMatch(fileNames.at(0)))
		{
            currentImage->pixmap().save(fileNames.at(0));
        }
		else
		{
            QMessageBox::information(this, "Information", "Save error: bad format or filename.");
        }
    }*/
}

void MainWindow::setupShortcuts()
{
    QList<QKeySequence> shortcuts;
    shortcuts << Qt::Key_Plus << Qt::Key_Equal;
    _zoomInAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Minus << Qt::Key_Underscore;
    _zoomOutAction->setShortcuts(shortcuts);
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
		_exitAction->setText(tr("E&xit"));
		_zoomInAction->setText(tr("Zoom &in"));
		_zoomOutAction->setText(tr("Zoom &out"));
		_chsAction->setText(tr("&Chinese"));
	}

	QMainWindow::changeEvent(event);
}