#include "ToolBar.h"

#include <QAction>
#include <QMenu>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>

#include "GlobalFunc.h"
#include "mainwindow.h"
#include "ToolButton.h"
#include "View.h"
#include "Document.h"
#include "MouseHandler.h"

ToolBar::ToolBar(QWidget* parent)
	: QToolBar(parent)
{
	createAction();
	createButton();
}

ToolBar::ToolBar(const QString& title, QWidget* parent)
	: QToolBar(title, parent)
{
	createAction();
	createButton();
}

ToolBar::~ToolBar()
{
}

void ToolBar::createAction()
{
	_openDicomAction = new QAction(tr("Open &DICOM file..."), this);
	_openRawAction = new QAction(tr("Open &Raw file..."), this);

	_showMenuAction = new QAction(tr("Menu"), this);
	_showMenuAction->setCheckable(true);
	_showMenuAction->setChecked(true);
	_showDockWidgetAction = new QAction(tr("Dock widgets"), this);
	_showDockWidgetAction->setCheckable(true);
	_showDockWidgetAction->setChecked(true);
	_fullScreenAction = new QAction(tr("Full screen mode"), this);
	_annotationAction = new QAction(tr("Annotations"), this);
	_annotationAction->setCheckable(true);
	_annotationAction->setChecked(true);
	_crossAction = new QAction(tr("Cross reference line"), this);
	_crossAction->setCheckable(true);
	_scaleAction = new QAction(tr("Image scale"), this);
	_scaleAction->setCheckable(true);
	_scaleAction->setChecked(true);

	_imageNegativeAction = new QAction(tr("Negative"), this);

	_fitWindowAction = new QAction(tr("Fit window"), this);
	_1xAction = new QAction(tr("100%"), this);
	_2xAction = new QAction(tr("200%"), this);
	_4xAction = new QAction(tr("400%"), this);
	_8xAction = new QAction(tr("800%"), this);
	_zoomInAction = new QAction(tr("Zoom in"), this);
	_zoomOutAction = new QAction(tr("Zoom out"), this);

	_cursorAction = new QAction(tr("Select"), this);
	_cursorAction->setIcon(QIcon("Resources/svg/cursor.svg"));
	_moveAction = new QAction(tr("Move"), this);
	_moveAction->setIcon(QIcon("Resources/svg/move.svg"));
	_rulerAction = new QAction(tr("Length"), this);
	_rulerAction->setIcon(QIcon("Resources/svg/ruler.svg"));
	_angleAction = new QAction(tr("Angle"), this);
	_angleAction->setIcon(QIcon("Resources/svg/angle.svg"));
	_rectAction = new QAction(tr("Rectangle"), this);
	_rectAction->setIcon(QIcon("Resources/svg/rectangle.svg"));
	_roundrectAction = new QAction(tr("Round Rectangle"), this);
	_roundrectAction->setIcon(QIcon("Resources/svg/roundrectangle.svg"));
	_circleAction = new QAction(tr("Circle"), this);
	_circleAction->setIcon(QIcon("Resources/svg/circle.svg"));
	_ellipseAction = new QAction(tr("Ellipse"), this);
	_ellipseAction->setIcon(QIcon("Resources/svg/ellipse.svg"));
	_rhombusAction = new QAction(tr("Rhombus"), this);
	_rhombusAction->setIcon(QIcon("Resources/svg/rhombus.svg"));
	_parallelogramAction = new QAction(tr("Parallelogram"), this);
	_parallelogramAction->setIcon(QIcon("Resources/svg/parallelogram.svg"));
	_arrowAction = new QAction(tr("Arrow"), this);
	_arrowAction->setIcon(QIcon("Resources/svg/arrow.svg"));

	MainWindow* mainWindow = getGlobalWindow();
	connect(_openDicomAction, &QAction::triggered, mainWindow, &MainWindow::openDicomImage);
	connect(_openRawAction, &QAction::triggered, mainWindow, &MainWindow::openRawImage);
	connect(_showMenuAction, &QAction::triggered, mainWindow, &MainWindow::showMenuBar);
	connect(_showDockWidgetAction, &QAction::triggered, mainWindow, &MainWindow::showDockWidget);
	connect(_fullScreenAction, &QAction::triggered, mainWindow, &MainWindow::fullScreen);
	connect(_annotationAction, &QAction::triggered, mainWindow, &MainWindow::showAnnotation);
	connect(_crossAction, &QAction::triggered, mainWindow, &MainWindow::showCrossLine);
	connect(_scaleAction, &QAction::triggered, mainWindow, &MainWindow::showScale);
	connect(_imageNegativeAction, &QAction::triggered, mainWindow->getDocument(), &Document::inverseImage);

	connect(_fitWindowAction, &QAction::triggered, mainWindow->getView(), &View::fitWindow);
	connect(_1xAction, &QAction::triggered, mainWindow->getView(), &View::zoomNormal);
	connect(_2xAction, &QAction::triggered, mainWindow->getView(), &View::zoom2x);
	connect(_4xAction, &QAction::triggered, mainWindow->getView(), &View::zoom4x);
	connect(_8xAction, &QAction::triggered, mainWindow->getView(), &View::zoom8x);
	connect(_zoomInAction, &QAction::triggered, mainWindow->getView(), &View::zoomIn);
	connect(_zoomOutAction, &QAction::triggered, mainWindow->getView(), &View::zoomOut);

	connect(_cursorAction, &QAction::triggered, this, &ToolBar::selectItem);
	connect(_moveAction, &QAction::triggered, this, &ToolBar::moveScene);
	connect(_rulerAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_angleAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_rectAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_roundrectAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_circleAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_ellipseAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_rhombusAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_parallelogramAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_arrowAction, &QAction::triggered, this, &ToolBar::measurementChanged);
}

void ToolBar::createButton()
{
	MainWindow* mainWindow = getGlobalWindow();

	_openToolButton = new QToolButton;
	_openToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	QMenu* menu = new QMenu(this);
	menu->addAction(_openDicomAction);
	menu->addAction(_openRawAction);
	_openToolButton->setMenu(menu);
	_openToolButton->setIcon(QIcon("Resources/svg/open.svg"));
	_openToolButton->setToolTip(tr("Open image file"));
	connect(_openToolButton, &QToolButton::clicked, mainWindow, &MainWindow::openImage);

	_saveToolButton = new QToolButton;
	_saveToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_openDicomAction);
	_saveToolButton->setMenu(menu);
	_saveToolButton->setIcon(QIcon("Resources/svg/save.svg"));
	_saveToolButton->setToolTip(tr("Save image file"));
	connect(_saveToolButton, &QToolButton::clicked, mainWindow, &MainWindow::saveAs);

	_layoutToolButton = new QToolButton;
	_layoutToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_showMenuAction);
	menu->addAction(_showDockWidgetAction);
	menu->addSeparator();
	menu->addAction(_fullScreenAction);
	_layoutToolButton->setMenu(menu);
	_layoutToolButton->setIcon(QIcon("Resources/svg/layout.svg"));
	_layoutToolButton->setToolTip(tr("Change layout"));
	connect(_layoutToolButton, &QToolButton::clicked, mainWindow, &MainWindow::showMenuBar);
	connect(_layoutToolButton, &QToolButton::clicked, mainWindow, &MainWindow::showDockWidget);

	_annotationToolButton = new QToolButton;
	_annotationToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_annotationAction);
	menu->addAction(_crossAction);
	menu->addAction(_scaleAction);
	_annotationToolButton->setMenu(menu);
	_annotationToolButton->setIcon(QIcon("Resources/svg/annotation.svg"));
	_annotationToolButton->setToolTip(tr("Toggle annotations"));
	connect(_annotationToolButton, &QToolButton::clicked, mainWindow, &MainWindow::saveAs);

	_imageWindowToolButton = new ToolButton;
	_imageWindowToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_imageNegativeAction);
	_imageWindowToolButton->setMenu(menu);
	_imageWindowToolButton->setIconByName("Resources/svg/imagewindow.svg");
	_imageWindowToolButton->setToolTip(tr("Adjust image window"));
	_imageWindowToolButton->installEventFilter(this);
	connect(_imageWindowToolButton, &QToolButton::clicked, this, &ToolBar::imageWindowToolButtonClicked);

	_zoomButton = new ToolButton;
	_zoomButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_fitWindowAction);
	menu->addSeparator();
	menu->addAction(_1xAction);
	menu->addAction(_2xAction);
	menu->addAction(_4xAction);
	menu->addAction(_8xAction);
	menu->addSeparator();
	menu->addAction(_zoomInAction);
	menu->addAction(_zoomOutAction);
	_zoomButton->setMenu(menu);
	_zoomButton->setIconByName("Resources/svg/zoom.svg");
	_zoomButton->setToolTip(tr("Zoom image"));
	_zoomButton->installEventFilter(this);
	connect(_zoomButton, &QToolButton::clicked, this, &ToolBar::zoomButtonClicked);

	_cursorButton = new ToolButton;
	_cursorButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_cursorAction);
	menu->addAction(_moveAction);
	_cursorButton->setMenu(menu);
	_cursorButton->setIconByName("Resources/svg/cursor.svg");
	_cursorButton->setToolTip(tr("Select item/Move image"));
	_cursorButton->installEventFilter(this);
	_cursorButton->setCurrentAction(_cursorAction);
	connect(_cursorButton, &QToolButton::triggered, this, &ToolBar::cursorButtonTriggered);
	connect(_cursorButton, &ToolButton::unbounded, this, &ToolBar::cursorButtonUnbounded);

	_measurementButton = new ToolButton;
	_measurementButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_rulerAction);
	menu->addAction(_angleAction);
	menu->addAction(_arrowAction);
	menu->addSeparator();
	menu->addAction(_rectAction);
	menu->addAction(_roundrectAction);
	menu->addAction(_circleAction);
	menu->addAction(_ellipseAction);
	menu->addAction(_rhombusAction);
	menu->addAction(_parallelogramAction);
	_measurementButton->setMenu(menu);
	_measurementButton->setIconByName("Resources/svg/ruler.svg");
	_measurementButton->setToolTip(tr("Measurements and tools"));
	_measurementButton->installEventFilter(this);
	_measurementButton->setCurrentAction(_rulerAction);
	connect(_measurementButton, &QToolButton::triggered, this, &ToolBar::measurementButtonTriggered);

	addWidget(_openToolButton);
	addWidget(_saveToolButton);
	addSeparator();
	addWidget(_layoutToolButton);
	addWidget(_annotationToolButton);
	addSeparator();
	addWidget(_imageWindowToolButton);
	addWidget(_zoomButton);
	addWidget(_cursorButton);
	addWidget(_measurementButton);
}

void ToolBar::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		_openDicomAction->setText(tr("Open &DICOM file..."));
		_openRawAction->setText(tr("Open &Raw file..."));
	}
}

bool ToolBar::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == _imageWindowToolButton || obj == _zoomButton || obj == _cursorButton || obj == _measurementButton)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			ToolButton* toolButton = static_cast<ToolButton*>(obj);
			if (toolButton == nullptr)
				return false;

			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			// Make sure the clicked location is on the button not the menu area
			if (mouseEvent->pos().x() < toolButton->size().width() - 12)
			{
				if (toolButton->currentAction())
				{
					toolButton->currentAction()->trigger();
				}
				else
				{
					toolButton->click();
				}

				if (mouseEvent->button() == Qt::LeftButton)
				{
					ToolButton::setLeftMouseButton(toolButton);
					MouseHandler::setLeftButton(toolButton);
				}
				else if (mouseEvent->button() == Qt::RightButton)
				{
					ToolButton::setRightMouseButton(toolButton);
					MouseHandler::setRightButton(toolButton);
				}
			}

			return false;
		}
		else
			return false;
	}
	else
	{
		return QToolBar::eventFilter(obj, event);
	}
}

void ToolBar::imageWindowToolButtonClicked()
{
	_imageWindowToolButton->setMouseHandler(new ImageWindowMouseHandler());
}

void ToolBar::zoomButtonClicked()
{
	_zoomButton->setMouseHandler(new ZoomMouseHandler());
}

void ToolBar::selectItem()
{
	getGlobalView()->setSceneMode(MOVE_ITEM);
	_cursorButton->setIconByName("Resources/svg/cursor.svg");
	_cursorButton->setMouseHandler(new SelectMouseHandler());
}

void ToolBar::moveScene()
{
	getGlobalView()->setSceneMode(MOVE_SCENE);
	_cursorButton->setIconByName("Resources/svg/move.svg");
	_cursorButton->setMouseHandler(new MoveMouseHandler());
}

void ToolBar::cursorButtonTriggered(QAction* action)
{
	_cursorButton->setCurrentAction(action);
	ToolButton::setLeftMouseButton(_cursorButton);
	MouseHandler::setLeftButton(_cursorButton);
}

void ToolBar::cursorButtonUnbounded()
{
	getGlobalView()->setSceneMode(NO_DRAG);
}

void ToolBar::measurementChanged()
{
	MainWindow* mainWindow = getGlobalWindow();
	mainWindow->setToolBoxWidgetVisible(true, false);

	QAction* action = qobject_cast<QAction*>(sender());
	if (action == _rulerAction)
	{
		_measurementButton->setIconByName("Resources/svg/ruler.svg");
		getGlobalView()->setItemType(DiagramItem::Line);
	}
	else if (action == _angleAction)
	{
		_measurementButton->setIconByName("Resources/svg/angle.svg");
	}
	else if (action == _arrowAction)
	{
		_measurementButton->setIconByName("Resources/svg/arrow.svg");
		getGlobalView()->setItemType(DiagramItem::Line);
	}
	else if (action == _rectAction)
	{
		_measurementButton->setIconByName("Resources/svg/rectangle.svg");
		getGlobalView()->setItemType(DiagramItem::Rect);
	}
	else if (action == _roundrectAction)
	{
		_measurementButton->setIconByName("Resources/svg/roundrectangle.svg");
		getGlobalView()->setItemType(DiagramItem::RoundRect);
	}
	else if (action == _circleAction)
	{
		_measurementButton->setIconByName("Resources/svg/circle.svg");
		getGlobalView()->setItemType(DiagramItem::Circle);
	}
	else if (action == _ellipseAction)
	{
		_measurementButton->setIconByName("Resources/svg/ellipse.svg");
		getGlobalView()->setItemType(DiagramItem::Ellipse);
	}
	else if (action == _rhombusAction)
	{
		_measurementButton->setIconByName("Resources/svg/rhombus.svg");
		getGlobalView()->setItemType(DiagramItem::Rhombus);
	}
	else if (action == _parallelogramAction)
	{
		_measurementButton->setIconByName("Resources/svg/parallelogram.svg");
		getGlobalView()->setItemType(DiagramItem::Parallelogram);
	}
	getGlobalView()->setSceneMode(INSERT_ITEM);
	_measurementButton->setMouseHandler(new DrawMouseHandler());
}

void ToolBar::measurementButtonTriggered(QAction* action)
{
	_measurementButton->setCurrentAction(action);
	ToolButton::setLeftMouseButton(_measurementButton);
	MouseHandler::setLeftButton(_measurementButton);
}