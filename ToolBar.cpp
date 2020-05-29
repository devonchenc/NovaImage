#include "ToolBar.h"

#include <QAction>
#include <QMenu>
#include <QToolButton>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

#include "GlobalFunc.h"
#include "mainwindow.h"

ToolBar::ToolBar(QWidget* parent)
	: QToolBar(parent)
	, _leftMouseButton(nullptr)
	, _rightMouseButton(nullptr)
{
	createAction();
	createButton();
}

ToolBar::ToolBar(const QString& title, QWidget* parent)
	: QToolBar(title, parent)
	, _leftMouseButton(nullptr)
	, _rightMouseButton(nullptr)
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
	_crossAction->setChecked(true);
	_scaleAction = new QAction(tr("Image scale"), this);
	_scaleAction->setCheckable(true);
	_scaleAction->setChecked(true);

	_imageWindowAction = new QAction(tr("Adjust image window"), this);
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

	MainWindow* mainWindow = getGlobalWindow();
	connect(_openDicomAction, &QAction::triggered, mainWindow, &MainWindow::openDicomImage);
	connect(_openRawAction, &QAction::triggered, mainWindow, &MainWindow::openRawImage);
	connect(_showMenuAction, &QAction::triggered, mainWindow, &MainWindow::showMenuBar);
	connect(_showDockWidgetAction, &QAction::triggered, mainWindow, &MainWindow::showDockWidget);
	connect(_fullScreenAction, &QAction::triggered, mainWindow, &MainWindow::fullScreen);
	connect(_annotationAction, &QAction::triggered, mainWindow, &MainWindow::showAnnotation);
	connect(_crossAction, &QAction::triggered, mainWindow, &MainWindow::showCrossLine);
	connect(_scaleAction, &QAction::triggered, mainWindow, &MainWindow::showScale);
	connect(_imageWindowAction, &QAction::triggered, mainWindow, &MainWindow::showScale);
	connect(_imageNegativeAction, &QAction::triggered, mainWindow, &MainWindow::showScale);

	connect(_zoomInAction, &QAction::triggered, mainWindow, &MainWindow::zoomIn);
	connect(_zoomOutAction, &QAction::triggered, mainWindow, &MainWindow::zoomOut);

	connect(_cursorAction, &QAction::triggered, this, &ToolBar::selectItem);
	connect(_moveAction, &QAction::triggered, this, &ToolBar::moveScene);
	connect(_rulerAction, &QAction::triggered, this, &ToolBar::measurementChanged);
	connect(_angleAction, &QAction::triggered, this, &ToolBar::measurementChanged);
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
	connect(_openToolButton, &QToolButton::clicked, mainWindow, &MainWindow::openImage);

	_saveToolButton = new QToolButton;
	_saveToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_openDicomAction);
	_saveToolButton->setMenu(menu);
	_saveToolButton->setIcon(QIcon("Resources/svg/save.svg"));
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
	connect(_annotationToolButton, &QToolButton::clicked, mainWindow, &MainWindow::saveAs);

	_imageWindowToolButton = new QToolButton;
	_imageWindowToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_imageNegativeAction);
	_imageWindowToolButton->setMenu(menu);
	_imageWindowToolButton->setIcon(QIcon("Resources/svg/imagewindow.svg"));
	_imageWindowToolButton->installEventFilter(this);

	_zoomButton = new QToolButton;
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
	_zoomButton->setIcon(QIcon("Resources/svg/zoom.svg"));
	_zoomButton->installEventFilter(this);

	_cursorButton = new QToolButton;
	_cursorButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_cursorAction);
	menu->addAction(_moveAction);
	_cursorButton->setMenu(menu);
	_cursorButton->setIcon(QIcon("Resources/svg/cursor.svg"));
	_cursorButton->installEventFilter(this);

	_measurementButton = new QToolButton;
	//	_measurementButton->setMinimumSize(QSize(40, 32));
	_measurementButton->setPopupMode(QToolButton::MenuButtonPopup);
	menu = new QMenu(this);
	menu->addAction(_rulerAction);
	menu->addAction(_angleAction);
	_measurementButton->setMenu(menu);
	_measurementButton->setIcon(QIcon("Resources/svg/ruler.svg"));
	connect(_measurementButton, &QToolButton::clicked, this, &ToolBar::measurementChanged);
	_measurementButton->installEventFilter(this);

	addWidget(_openToolButton);
	addWidget(_saveToolButton);
	addSeparator();
	addWidget(_layoutToolButton);
	addWidget(_annotationToolButton);
	addWidget(_imageWindowToolButton);
	addWidget(_zoomButton);
	addSeparator();
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
			QToolButton* toolButton = static_cast<QToolButton*>(obj);
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			if (mouseEvent->button() == Qt::LeftButton)
			{
				_leftMouseButton = toolButton;
			}
			else if (mouseEvent->button() == Qt::RightButton)
			{
				_rightMouseButton = toolButton;
			}
			updateToolButtonIcon();
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

void ToolBar::selectItem()
{
	_cursorButton->setIcon(QIcon("Resources/svg/cursor.svg"));
//	_view->setSceneMode(SELECT_ITEM);
}

void ToolBar::moveScene()
{
	_cursorButton->setIcon(QIcon("Resources/svg/move.svg"));
//	_view->setSceneMode(MOVE_SCENE);
}

void ToolBar::measurementChanged()
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

void ToolBar::updateToolButtonIcon()
{
	_imageWindowToolButton->setIcon(createToolButtonIcon("Resources/svg/imagewindow.svg", _imageWindowToolButton == _leftMouseButton, _imageWindowToolButton == _rightMouseButton));
	_zoomButton->setIcon(createToolButtonIcon("Resources/svg/zoom.svg", _zoomButton == _leftMouseButton, _zoomButton == _rightMouseButton));
	_cursorButton->setIcon(createToolButtonIcon("Resources/svg/cursor.svg", _cursorButton == _leftMouseButton, _cursorButton == _rightMouseButton));
	_measurementButton->setIcon(createToolButtonIcon("Resources/svg/ruler.svg", _measurementButton == _leftMouseButton, _measurementButton == _rightMouseButton));
}

QIcon ToolBar::createToolButtonIcon(const QString& imageFile, bool left, bool right)
{
	int width = 32;
	int height = 32;
	QIcon icon = QIcon(imageFile);
	QPixmap pixmap = icon.pixmap(width, height);

	if (left || right)
	{
		QPainter painter(&pixmap);
		QPen newPen(qRgb(96, 96, 96));
		painter.setPen(QPen(qRgb(96, 96, 96)));
		painter.setBrush(QBrush(qRgb(224, 224, 224)));

		int offset = 1;
		painter.drawRect(QRect(0, 16 + offset, 10, 15 - offset));
		painter.drawRect(QRect(0, 16 + offset, 5, 6));
		painter.drawRect(QRect(5, 16 + offset, 5, 6));
		
		if (left)
		{
			painter.fillRect(QRect(1, 17 + offset, 4, 5), qRgb(255, 216, 0));
		}
		if (right)
		{
			painter.fillRect(QRect(6, 17 + offset, 4, 5), qRgb(255, 148, 166));
		}
	}

	
	return QIcon(pixmap);
}