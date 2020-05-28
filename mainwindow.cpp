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
	_showMenuAction = new QAction(tr("Menu"), this);
	_showMenuAction->setCheckable(true);
	_showMenuAction->setChecked(true);
	_showDockWidgetAction = new QAction(tr("Dock widgets"), this);
	_showDockWidgetAction->setCheckable(true);
	_showDockWidgetAction->setChecked(true);
	_annotationAction = new QAction(tr("Annotations"), this);
	_annotationAction->setCheckable(true);
	_annotationAction->setChecked(true);
	_crossAction = new QAction(tr("Cross reference line"), this);
	_crossAction->setCheckable(true);
	_crossAction->setChecked(true);
	_scaleAction = new QAction(tr("Image scale"), this);
	_scaleAction->setCheckable(true);
	_scaleAction->setChecked(true);
	_cursorAction = new QAction(tr("Select"), this);
	_cursorAction->setIcon(QIcon("Resources/svg/cursor.svg"));
	_moveAction = new QAction(tr("Move"), this);
	_moveAction->setIcon(QIcon("Resources/svg/move.svg"));
	_rulerAction = new QAction(tr("Length"), this);
	_rulerAction->setIcon(QIcon("Resources/svg/ruler.svg"));
	_angleAction = new QAction(tr("Angle"), this);
	_angleAction->setIcon(QIcon("Resources/svg/angle.svg"));
	_fullScreenAction = new QAction(tr("Full screen mode"), this);
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
	connect(_showMenuAction, &QAction::triggered, this, &MainWindow::showMenu);
	connect(_showDockWidgetAction, &QAction::triggered, this, &MainWindow::showDockWidget);
	connect(_fullScreenAction, &QAction::triggered, this, &MainWindow::fullScreen);
	connect(_annotationAction, &QAction::triggered, this, &MainWindow::showAnnotation);
	connect(_crossAction, &QAction::triggered, this, &MainWindow::showCrossLine);
	connect(_scaleAction, &QAction::triggered, this, &MainWindow::showScale);

	connect(_cursorAction, &QAction::triggered, this, &MainWindow::selectItem);
	connect(_moveAction, &QAction::triggered, this, &MainWindow::moveScene);
	connect(_rulerAction, &QAction::triggered, this, &MainWindow::measurementChanged);
	connect(_angleAction, &QAction::triggered, this, &MainWindow::measurementChanged);

	connect(_zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);
	connect(_zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);
	connect(_prevImageAction, &QAction::triggered, this, &MainWindow::prevImage);
	connect(_nextImageAction, &QAction::triggered, this, &MainWindow::nextImage);

	setupShortcuts();
}

void MainWindow::createToolbar()
{
	setIconSize(QSize(42, 32));

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
	menu->addAction(_showMenuAction);
	menu->addAction(_showDockWidgetAction);
	menu->addSeparator();
	menu->addAction(_fullScreenAction);
	_layoutToolButton->setMenu(menu);
	_layoutToolButton->setIcon(QIcon("Resources/svg/layout.svg"));
	connect(_layoutToolButton, &QToolButton::clicked, _saveAsAction, &QAction::triggered);

	_annotationToolButton = new QToolButton;
	_annotationToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_annotationAction);
	menu->addAction(_crossAction);
	menu->addAction(_scaleAction);
	_annotationToolButton->setMenu(menu);
	_annotationToolButton->setIcon(QIcon("Resources/svg/annotation.svg"));
	connect(_annotationToolButton, &QToolButton::clicked, _saveAsAction, &QAction::triggered);

	_imageWindowToolButton = new QToolButton;
	_imageWindowToolButton->setPopupMode(QToolButton::MenuButtonPopup);
/*	menu = new QMenu(this);
	menu->addAction(_annotationAction);
	menu->addAction(_crossAction);
	menu->addAction(_scaleAction);
	_annotationToolButton->setMenu(menu);*/
	_imageWindowToolButton->setIcon(QIcon("Resources/svg/imagewindow.svg"));
	connect(_imageWindowToolButton, &QToolButton::clicked, _saveAsAction, &QAction::triggered);

	_zoomButton = new QToolButton;
	_zoomButton->setPopupMode(QToolButton::MenuButtonPopup);
	/*	menu = new QMenu(this);
		menu->addAction(_annotationAction);
		menu->addAction(_crossAction);
		menu->addAction(_scaleAction);
		_zoomButton->setMenu(menu);*/
	_zoomButton->setIcon(QIcon("Resources/svg/zoom.svg"));
	connect(_zoomButton, &QToolButton::clicked, _saveAsAction, &QAction::triggered);

	_cursorButton = new QToolButton;
	_cursorButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_cursorAction);
	menu->addAction(_moveAction);
	_cursorButton->setMenu(menu);
	_cursorButton->setIcon(QIcon("Resources/svg/cursor.svg"));
	connect(_cursorButton, &QToolButton::clicked, _saveAsAction, &QAction::triggered);

	_measurementButton = new QToolButton;
//	_measurementButton->setMinimumSize(QSize(40, 32));
	_measurementButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_rulerAction);
	menu->addAction(_angleAction);
	_measurementButton->setMenu(menu);
	_measurementButton->setIcon(QIcon("Resources/svg/ruler.svg"));
	connect(_measurementButton, &QToolButton::clicked, this, &MainWindow::measurementChanged);

	_fileToolBar->addWidget(_openToolButton);
	_fileToolBar->addWidget(_saveToolButton);
	_fileToolBar->addSeparator();
	_fileToolBar->addWidget(_layoutToolButton);
	_fileToolBar->addWidget(_annotationToolButton);
	_fileToolBar->addWidget(_imageWindowToolButton);
	_fileToolBar->addWidget(_zoomButton);
	_fileToolBar->addSeparator();
	_fileToolBar->addWidget(_cursorButton);
	_fileToolBar->addWidget(_measurementButton);

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
	connect(_view, &View::showInfo, infoLabel, &QLabel::setText);

	QLabel* coordinateLabel = new QLabel(mainStatusBar);
	coordinateLabel->setMinimumWidth(100);
	coordinateLabel->setAlignment(Qt::AlignHCenter);
	connect(_view->view(), &GraphicsView::showCoordinate, coordinateLabel, &QLabel::setText);

	QLabel* pixelValueLabel = new QLabel(mainStatusBar);
	pixelValueLabel->setMinimumWidth(120);
	pixelValueLabel->setAlignment(Qt::AlignHCenter);
	connect(_view->view(), &GraphicsView::showPixelValue, pixelValueLabel, &QLabel::setText);

	QToolButton* fitScreenButton = new QToolButton(mainStatusBar);
	fitScreenButton->setText("FitScreen");
	connect(fitScreenButton, &QToolButton::clicked, _view, &View::fitScreen);
	QToolButton* normalButton = new QToolButton(mainStatusBar);
	normalButton->setText("Normal");
	connect(normalButton, &QToolButton::clicked, _view->view(), &GraphicsView::zoomNormal);

	QToolButton* zoomOutButton = new QToolButton(mainStatusBar);
	zoomOutButton->setText("-");
	connect(zoomOutButton, &QToolButton::clicked, _view->view(), &GraphicsView::zoomOut);
	QToolButton* zoomInButton = new QToolButton(mainStatusBar);
	zoomInButton->setText("+");
	connect(zoomInButton, &QToolButton::clicked, _view->view(), &GraphicsView::zoomIn);

	_slider = new QSlider(mainStatusBar);
	_slider->setOrientation(Qt::Horizontal);
	_slider->setMinimum(0);
	_slider->setMaximum(300);
	_slider->setSingleStep(5);
	_slider->setValue(150);
	_slider->setMaximumWidth(200);
	_slider->setTickPosition(QSlider::TicksBelow);
	connect(_view->view(), &GraphicsView::valueChanged, _slider, &QSlider::setValue);
	connect(_slider, &QSlider::valueChanged, _view->view(), &GraphicsView::setValue);

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
	connect(toolbox, &ToolBoxWidget::setSceneMode, _view, &View::setSceneMode);
	connect(toolbox, &ToolBoxWidget::setItemType, _view->scene(), &GraphicsScene::setItemType);
	connect(toolbox, &ToolBoxWidget::setLineColor, _view->scene(), &GraphicsScene::setLineColor);
	connect(toolbox, &ToolBoxWidget::setFillColor, _view->scene(), &GraphicsScene::setFillColor);
	connect(toolbox, &ToolBoxWidget::setTextColor, _view->scene(), &GraphicsScene::setTextColor);
	connect(toolbox, &ToolBoxWidget::setTextFont, _view->scene(), &GraphicsScene::setTextFont);

	connect(_view->scene(), &GraphicsScene::itemInserted, toolbox, &ToolBoxWidget::itemInserted);
	connect(_view->scene(), &GraphicsScene::itemSelected, toolbox, &ToolBoxWidget::itemSelected);
	connect(_view->scene(), &GraphicsScene::textSelected, toolbox, &ToolBoxWidget::textSelected);

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
	filters << tr("DICOM files (*.dcm)")
		<< tr("Image files (*.png *.bmp *.jpg *.tif)")
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

void MainWindow::showMenu()
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

}

void MainWindow::showScale()
{

}

void MainWindow::selectItem()
{
	_cursorButton->setIcon(QIcon("Resources/svg/cursor.svg"));
	_view->setSceneMode(SELECT_ITEM);
}

void MainWindow::moveScene()
{
	_cursorButton->setIcon(QIcon("Resources/svg/move.svg"));
	_view->setSceneMode(MOVE_SCENE);
}

void MainWindow::measurementChanged()
{
	QAction* action = qobject_cast<QAction *>(sender());
	if (action == _rulerAction)
	{
		_measurementButton->setIcon(QIcon("Resources/svg/ruler.svg"));
	}
	else if (action == _angleAction)
	{
		_measurementButton->setIcon(QIcon("Resources/svg/angle.svg"));
	}
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