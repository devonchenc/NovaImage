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
	_saveAsRawAction = new QAction(tr("Save as &Raw file..."), this);

	_showMenuAction = new QAction(tr("Menu"), this);
	_showMenuAction->setCheckable(true);
	_showMenuAction->setChecked(true);
	_showDockWidgetAction = new QAction(tr("Dock widgets"), this);
	_showDockWidgetAction->setCheckable(true);
	_showDockWidgetAction->setChecked(true);
	_fullScreenAction = new QAction(tr("Full screen mode"), this);
	_showAnnotationAction = new QAction(tr("Annotations"), this);
	_showAnnotationAction->setCheckable(true);
	_showAnnotationAction->setChecked(true);
	_showCrossAction = new QAction(tr("Cross reference line"), this);
	_showCrossAction->setCheckable(true);
	_showScaleAction = new QAction(tr("Image scale"), this);
	_showScaleAction->setCheckable(true);
	_showScaleAction->setChecked(true);
	_showMeasurementAction = new QAction(tr("Measurements"), this);
	_showMeasurementAction->setCheckable(true);
	_showMeasurementAction->setChecked(true);

	_flipHorizontalAction = new QAction(tr("Flip horizontal"), this);
	_flipHorizontalAction->setCheckable(true);
	_flipVerticalAction = new QAction(tr("Flip vertical"), this);
	_flipVerticalAction->setCheckable(true);

	_rotate90CW = new QAction(tr("Rotate 90 ") + QString(QChar(0x00B0)) + tr(" CW"), this);	//0x00B0:degree sign
	_rotate90CCW = new QAction(tr("Rotate 90 ") + QString(QChar(0x00B0)) + tr(" CCW"), this);
	_rotate180 = new QAction(tr("Rotate 180 ") + QString(QChar(0x00B0)), this);
	_resetTransformation = new QAction(tr("Reset transformation"), this);

	_restoreImageWindow = new QAction(tr("Default window"), this);
	_imageNegativeAction = new QAction(tr("Negative"), this);

	_fitWindowAction = new QAction(tr("Fit window"), this);
	_1xAction = new QAction("100%", this);
	_2xAction = new QAction("200%", this);
	_4xAction = new QAction("400%", this);
	_8xAction = new QAction("800%", this);
	_zoomInAction = new QAction(tr("Zoom in"), this);
	_zoomOutAction = new QAction(tr("Zoom out"), this);

	_cursorAction = new QAction(tr("Select"), this);
	_cursorAction->setIcon(QIcon("Resources/svg/cursor.svg"));
	_moveAction = new QAction(tr("Move"), this);
	_moveAction->setIcon(QIcon("Resources/svg/move.svg"));
	_rulerAction = new QAction(tr("Length"), this);
	_rulerAction->setIcon(QIcon("Resources/svg/length.svg"));
	_arrowAction = new QAction(tr("Arrow"), this);
	_arrowAction->setIcon(QIcon("Resources/svg/arrow.svg"));
	_angleAction = new QAction(tr("Angle"), this);
	_angleAction->setIcon(QIcon("Resources/svg/angle.svg"));
	_rectAction = new QAction(tr("Rectangle"), this);
	_rectAction->setIcon(QIcon("Resources/svg/rectangle.svg"));
	_roundrectAction = new QAction(tr("Round Rectangle"), this);
	_roundrectAction->setIcon(QIcon("Resources/svg/roundrect.svg"));
	_circleAction = new QAction(tr("Circle"), this);
	_circleAction->setIcon(QIcon("Resources/svg/circle.svg"));
	_ellipseAction = new QAction(tr("Ellipse"), this);
	_ellipseAction->setIcon(QIcon("Resources/svg/ellipse.svg"));
	_rhombusAction = new QAction(tr("Rhombus"), this);
	_rhombusAction->setIcon(QIcon("Resources/svg/rhombus.svg"));
	_parallelogramAction = new QAction(tr("Parallelogram"), this);
	_parallelogramAction->setIcon(QIcon("Resources/svg/parallelogram.svg"));
	_textAction = new QAction(tr("Text"), this);
	_textAction->setIcon(QIcon("Resources/svg/text.svg"));

	MainWindow* mainWindow = getGlobalWindow();
	connect(_openDicomAction, &QAction::triggered, mainWindow, &MainWindow::openDicomImage);
	connect(_openRawAction, &QAction::triggered, mainWindow, &MainWindow::openRawImage);
	connect(_saveAsRawAction, &QAction::triggered, mainWindow, &MainWindow::saveAsRawImage);

	connect(_showMenuAction, &QAction::toggled, mainWindow, &MainWindow::showMenuBar);
	connect(_showDockWidgetAction, &QAction::toggled, mainWindow, &MainWindow::showDockWidget);
	connect(_fullScreenAction, &QAction::triggered, mainWindow, &MainWindow::fullScreen);

	connect(_showAnnotationAction, &QAction::toggled, mainWindow, &MainWindow::showAnnotation);
	connect(_showCrossAction, &QAction::toggled, mainWindow, &MainWindow::showCrossLine);
	connect(_showScaleAction, &QAction::toggled, mainWindow, &MainWindow::showScale);
	connect(_showMeasurementAction, &QAction::toggled, mainWindow, &MainWindow::showMeasurement);

	connect(_flipHorizontalAction, &QAction::triggered, mainWindow->getView(), &View::flipHorizontal);
	connect(_flipVerticalAction, &QAction::triggered, mainWindow->getView(), &View::flipVertical);

	connect(_rotate90CW, &QAction::triggered, mainWindow->getView(), &View::rotate90CW);
	connect(_rotate90CCW, &QAction::triggered, mainWindow->getView(), &View::rotate90CCW);
	connect(_rotate180, &QAction::triggered, mainWindow->getView(), &View::rotate180);
	connect(_resetTransformation, &QAction::triggered, mainWindow->getView(), &View::resetTransformation);

	connect(_restoreImageWindow, &QAction::triggered, mainWindow->getDocument(), &Document::restoreImageWindow);
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
	connect(_arrowAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_angleAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_rectAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_roundrectAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_circleAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_ellipseAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_rhombusAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_parallelogramAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_textAction, &QAction::triggered, this, &ToolBar::measurementChanged);
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
	menu->addAction(_saveAsRawAction);
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
	connect(_layoutToolButton, &QToolButton::clicked, this, &ToolBar::layoutToolButtonClicked);

	_showInfoToolButton = new QToolButton;
	_showInfoToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_showAnnotationAction);
	menu->addAction(_showCrossAction);
	menu->addAction(_showScaleAction);
	menu->addAction(_showMeasurementAction);
	_showInfoToolButton->setMenu(menu);
	_showInfoToolButton->setIcon(QIcon("Resources/svg/annotation.svg"));
	_showInfoToolButton->setToolTip(tr("Toggle annotations"));
	connect(_showInfoToolButton, &QToolButton::clicked, this, &ToolBar::showInfoToolButtonClicked);

	_flipToolButton = new QToolButton;
	_flipToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_flipHorizontalAction);
	menu->addAction(_flipVerticalAction);
	_flipToolButton->setMenu(menu);
	_flipToolButton->setIcon(QIcon("Resources/svg/flip_horizontal.svg"));
	_flipToolButton->setToolTip(tr("Flip"));
	connect(_flipToolButton, &QToolButton::clicked, _flipHorizontalAction, &QAction::triggered);

	_rotateToolButton = new QToolButton;
	_rotateToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_rotate90CW);
	menu->addAction(_rotate90CCW);
	menu->addSeparator();
	menu->addAction(_rotate180);
	menu->addSeparator();
	menu->addAction(_resetTransformation);
	_rotateToolButton->setMenu(menu);
	_rotateToolButton->setIcon(QIcon("Resources/svg/rotate_cw.svg"));
	_rotateToolButton->setToolTip(tr("Rotate"));
	connect(_rotateToolButton, &QToolButton::clicked, _rotate90CW, &QAction::triggered);

	_imageWindowToolButton = new ToolButton;
	_imageWindowToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_restoreImageWindow);
	menu->addSeparator();
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
	menu->addAction(_arrowAction);
	menu->addAction(_angleAction);
	menu->addSeparator();
	menu->addAction(_rectAction);
	menu->addAction(_roundrectAction);
	menu->addAction(_circleAction);
	menu->addAction(_ellipseAction);
	menu->addAction(_rhombusAction);
	menu->addAction(_parallelogramAction);
	menu->addSeparator();
	menu->addAction(_textAction);
	_measurementButton->setMenu(menu);
	_measurementButton->setIconByName("Resources/svg/length.svg");
	_measurementButton->setToolTip(tr("Measurements and tools"));
	_measurementButton->installEventFilter(this);
	_measurementButton->setCurrentAction(_rulerAction);
	connect(_measurementButton, &QToolButton::triggered, this, &ToolBar::measurementButtonTriggered);

	addWidget(_openToolButton);
	addWidget(_saveToolButton);
	addSeparator();
	addWidget(_layoutToolButton);
	addWidget(_showInfoToolButton);
	addWidget(_flipToolButton);
	addWidget(_rotateToolButton);
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
		_saveAsRawAction->setText(tr("Save as &Raw file..."));

		_showMenuAction->setText(tr("Menu"));
		_showDockWidgetAction->setText(tr("Dock widgets"));
		_fullScreenAction->setText(tr("Full screen mode"));
		_showAnnotationAction->setText(tr("Annotations"));
		_showCrossAction->setText(tr("Cross reference line"));
		_showScaleAction->setText(tr("Image scale"));
		_showMeasurementAction->setText(tr("Measurements"));

		_flipHorizontalAction->setText(tr("Flip horizontal"));
		_flipVerticalAction->setText(tr("Flip vertical"));

		_rotate90CW->setText(tr("Rotate 90 ") + QString(QChar(0x00B0)) + tr(" CW"));
		_rotate90CCW->setText(tr("Rotate 90 ") + QString(QChar(0x00B0)) + tr(" CCW"));
		_rotate180->setText(tr("Rotate 180 ") + QString(QChar(0x00B0)));

		_restoreImageWindow->setText(tr("Default window"));
		_imageNegativeAction->setText(tr("Negative"));

		_fitWindowAction->setText(tr("Fit window"));
		_zoomInAction->setText(tr("Zoom in"));
		_zoomOutAction->setText(tr("Zoom out"));

		_cursorAction->setText(tr("Select"));
		_moveAction->setText(tr("Move"));
		_rulerAction->setText(tr("Length"));
		_arrowAction->setText(tr("Arrow"));
		_angleAction->setText(tr("Angle"));
		_rectAction->setText(tr("Rectangle"));
		_roundrectAction->setText(tr("Round Rectangle"));
		_circleAction->setText(tr("Circle"));
		_ellipseAction->setText(tr("Ellipse"));
		_rhombusAction->setText(tr("Rhombus"));
		_parallelogramAction->setText(tr("Parallelogram"));
		_textAction->setText(tr("Text"));

		_openToolButton->setToolTip(tr("Open image file"));
		_saveToolButton->setToolTip(tr("Save image file"));
		_layoutToolButton->setToolTip(tr("Change layout"));
		_showInfoToolButton->setToolTip(tr("Toggle annotations"));
		_flipToolButton->setToolTip(tr("Flip"));
		_rotateToolButton->setToolTip(tr("Rotate"));
		_imageWindowToolButton->setToolTip(tr("Adjust image window"));
		_zoomButton->setToolTip(tr("Zoom image"));
		_cursorButton->setToolTip(tr("Select item/Move image"));
		_measurementButton->setToolTip(tr("Measurements and tools"));
	}

	QToolBar::changeEvent(event);
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
				if (mouseEvent->button() == Qt::LeftButton)
					ToolButton::unsetLeftMouseButton();
				else if (mouseEvent->button() == Qt::RightButton)
					ToolButton::unsetRightMouseButton();

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

void ToolBar::setMeasurementType(DiagramItem::DiagramType type)
{
	switch (type)
	{
	case DiagramItem::None:
		_cursorAction->trigger();
		_cursorButton->triggered(_cursorAction);
		break;
	case DiagramItem::Line:
		_rulerAction->trigger();
		_measurementButton->triggered(_rulerAction);
		break;
	case DiagramItem::Arrow:
		_arrowAction->trigger();
		_measurementButton->triggered(_arrowAction);
		break;
	case DiagramItem::Angle:
		_angleAction->trigger();
		_measurementButton->triggered(_angleAction);
		break;
	case DiagramItem::Rect:
		_rectAction->trigger();
		_measurementButton->triggered(_rectAction);
		break;
	case DiagramItem::RoundRect:
		_roundrectAction->trigger();
		_measurementButton->triggered(_roundrectAction);
		break;
	case DiagramItem::Circle:
		_circleAction->trigger();
		_measurementButton->triggered(_circleAction);
		break;
	case DiagramItem::Ellipse:
		_ellipseAction->trigger();
		_measurementButton->triggered(_ellipseAction);
		break;
	case DiagramItem::Rhombus:
		_rhombusAction->trigger();
		_measurementButton->triggered(_rhombusAction);
		break;
	case DiagramItem::Parallelogram:
		_parallelogramAction->trigger();
		_measurementButton->triggered(_parallelogramAction);
		break;
	case DiagramItem::Text:
		_textAction->trigger();
		_measurementButton->triggered(_textAction);
		break;
	}
}

void ToolBar::layoutToolButtonClicked()
{
	bool checked = !_showMenuAction->isChecked();

	_showMenuAction->setChecked(checked);
	_showDockWidgetAction->setChecked(checked);

	_showMenuAction->toggled(checked);
	_showDockWidgetAction->toggled(checked);
}

void ToolBar::showInfoToolButtonClicked()
{
	bool checked = !_showAnnotationAction->isChecked();

	_showAnnotationAction->setChecked(checked);
	_showScaleAction->setChecked(checked);
	_showMeasurementAction->setChecked(checked);

	_showAnnotationAction->toggled(checked);
	_showScaleAction->toggled(checked);
	_showMeasurementAction->toggled(checked);
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
		_measurementButton->setIconByName("Resources/svg/length.svg");
		getGlobalView()->setItemType(DiagramItem::Line);
	}
	else if (action == _arrowAction)
	{
		_measurementButton->setIconByName("Resources/svg/arrow.svg");
		getGlobalView()->setItemType(DiagramItem::Arrow);
	}
	else if (action == _angleAction)
	{
		_measurementButton->setIconByName("Resources/svg/angle.svg");
		getGlobalView()->setItemType(DiagramItem::Angle);
	}
	else if (action == _rectAction)
	{
		_measurementButton->setIconByName("Resources/svg/rectangle.svg");
		getGlobalView()->setItemType(DiagramItem::Rect);
	}
	else if (action == _roundrectAction)
	{
		_measurementButton->setIconByName("Resources/svg/roundrect.svg");
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
	else if (action == _textAction)
	{
		_measurementButton->setIconByName("Resources/svg/text.svg");
		getGlobalView()->setItemType(DiagramItem::Text);
		mainWindow->setToolBoxWidgetVisible(false, true);
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