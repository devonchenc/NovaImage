#include "ToolBar.h"

#include <QAction>
#include <QMenu>
#include <QEvent>
#include <QMouseEvent>

#include "GlobalFunc.h"
#include "mainwindow.h"
#include "ToolButton.h"
#include "View.h"
#include "Document.h"
#include "MouseHandler.h"

ToolBar::ToolBar(QWidget* parent)
    : QToolBar(parent)
{
    createButton();
    createAction();
    initButton();
}

ToolBar::ToolBar(const QString& title, QWidget* parent)
    : QToolBar(title, parent)
{
    createButton();
    createAction();
    initButton();
}

ToolBar::~ToolBar()
{
}

void ToolBar::enableButton(bool flag)
{
    _saveButton->setEnabled(flag);
    _printButton->setEnabled(flag);
    _flipButton->setEnabled(flag);
    _rotateButton->setEnabled(flag);
    _cineButton->setEnabled(flag);

    _sliceButton->setEnabled(flag);
    _locationButton->setEnabled(flag);
    _imageWindowButton->setEnabled(flag);
    _zoomButton->setEnabled(flag);
    _cursorButton->setEnabled(flag);
    _measurementButton->setEnabled(flag);
    _undoButton->setEnabled(flag);
    _restoreButton->setEnabled(flag);
}

void ToolBar::enableViewAction(bool flag)
{
    _threeViewAction->setEnabled(flag);
    _fourViewAction->setEnabled(flag);
    _volumeViewAction->setEnabled(flag);
}

void ToolBar::checkViewAction(int type)
{
    _singleViewAction->setChecked(type == 0);
    _threeViewAction->setChecked(type == 1);
    _fourViewAction->setChecked(type == 2);
    _volumeViewAction->setChecked(type == 3);
}

void ToolBar::createButton()
{
    _openButton = new QToolButton;
    _openButton->setPopupMode(QToolButton::MenuButtonPopup);

    _saveButton = new QToolButton;
    _saveButton->setPopupMode(QToolButton::MenuButtonPopup);

    _printButton = new QToolButton;

    _layoutButton = new QToolButton;
    _layoutButton->setPopupMode(QToolButton::MenuButtonPopup);

    _showInfoButton = new QToolButton;
    _showInfoButton->setPopupMode(QToolButton::MenuButtonPopup);

    _flipButton = new QToolButton;
    _flipButton->setPopupMode(QToolButton::MenuButtonPopup);

    _rotateButton = new QToolButton;
    _rotateButton->setPopupMode(QToolButton::MenuButtonPopup);

    _cineButton = new QToolButton;
    _cineButton->setPopupMode(QToolButton::MenuButtonPopup);
    _cineButton->setCheckable(true);

    _sliceButton = new ToolButton;

    _locationButton = new ToolButton;

    _imageWindowButton = new ToolButton;
    _imageWindowButton->setPopupMode(QToolButton::MenuButtonPopup);

    _zoomButton = new ToolButton;
    _zoomButton->setPopupMode(QToolButton::MenuButtonPopup);

    _cursorButton = new ToolButton;
    _cursorButton->setPopupMode(QToolButton::MenuButtonPopup);

    _measurementButton = new ToolButton;
    _measurementButton->setPopupMode(QToolButton::MenuButtonPopup);

    _undoButton = new QToolButton;
//  _undoButton->setPopupMode(QToolButton::MenuButtonPopup);

    _restoreButton = new QToolButton;

    addWidget(_openButton);
    addWidget(_saveButton);
    addWidget(_printButton);
    addSeparator();
    addWidget(_layoutButton);
    addWidget(_showInfoButton);
    addWidget(_flipButton);
    addWidget(_rotateButton);
    addWidget(_cineButton);
    addSeparator();
    addWidget(_sliceButton);
    addWidget(_locationButton);
    addWidget(_imageWindowButton);
    addWidget(_zoomButton);
    addWidget(_cursorButton);
    addWidget(_measurementButton);
    addSeparator();
    addWidget(_undoButton);
    addWidget(_restoreButton);

    enableButton(false);
}

void ToolBar::createAction()
{
    _openDicomAction = new QAction(tr("Open &DICOM File..."), this);
    _openDicomFolderAction = new QAction(tr("Open DICOM &Folder..."), this);
    _openRawAction = new QAction(tr("Open &Raw File..."), this);
    _saveAsRawAction = new QAction(tr("Save As &Raw File..."), this);

    _showMenuAction = new QAction(tr("Menu"), this);
    _showMenuAction->setCheckable(true);
    _showMenuAction->setChecked(true);
    _showDockWidgetAction = new QAction(tr("Dock Widgets"), this);
    _showDockWidgetAction->setCheckable(true);
    _showDockWidgetAction->setChecked(true);
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
    _fullScreenAction = new QAction(tr("Full Screen Mode"), this);
    _fullScreenAction->setIcon(QIcon(":/icon/svg/fullscreen.svg"));

    _showAnnotationAction = new QAction(tr("Annotations"), this);
    _showAnnotationAction->setCheckable(true);
    _showAnnotationAction->setChecked(true);
    _showCrossAction = new QAction(tr("Cross Reference Line"), this);
    _showCrossAction->setCheckable(true);
    _showScaleAction = new QAction(tr("Image Scale"), this);
    _showScaleAction->setCheckable(true);
    _showScaleAction->setChecked(true);
    _showMeasurementAction = new QAction(tr("Measurements"), this);
    _showMeasurementAction->setCheckable(true);
    _showMeasurementAction->setChecked(true);

    _flipHorizontalAction = new QAction(tr("Flip Horizontal"), this);
    _flipHorizontalAction->setCheckable(true);
    _flipHorizontalAction->setIcon(QIcon(":/icon/svg/flip_horizontal.svg"));
    _flipVerticalAction = new QAction(tr("Flip Vertical"), this);
    _flipVerticalAction->setCheckable(true);
    _flipVerticalAction->setIcon(QIcon(":/icon/svg/flip_vertical.svg"));

    _rotate90CWAction = new QAction(tr("Rotate 90 ") + QString(QChar(0x00B0)) + tr(" CW"), this); //0x00B0:degree sign
    _rotate90CWAction->setIcon(QIcon(":/icon/svg/rotate_cw.svg"));
    _rotate90CCWAction = new QAction(tr("Rotate 90 ") + QString(QChar(0x00B0)) + tr(" CCW"), this);
    _rotate90CCWAction->setIcon(QIcon(":/icon/svg/rotate_ccw.svg"));
    _rotate180Action = new QAction(tr("Rotate 180 ") + QString(QChar(0x00B0)), this);
    _resetTransformation = new QAction(tr("Reset Transformation"), this);
    _resetTransformation->setIcon(QIcon(":/icon/svg/reset.svg"));

    _FPS30Action = new QAction("30 FPS", this);
    _FPS30Action->setCheckable(true);
    _FPS30Action->setChecked(true);
    _FPS60Action = new QAction("60 FPS", this);
    _FPS60Action->setCheckable(true);
    _FPS60Action->setChecked(false);

    _sliceAction = new QAction(tr("Browse slices"), _sliceButton);
    _sliceAction->setObjectName("slice");
    _sliceAction->setIcon(QIcon(":/icon/svg/slice.svg"));

    _locationAction = new QAction(tr("Location"), _locationButton);
    _locationAction->setObjectName("location");
    _locationAction->setIcon(QIcon(":/icon/svg/location.svg"));

    _imageWindowAction = new QAction(tr("Adjust Window"), _imageWindowButton);
    _imageWindowAction->setObjectName("imagewindow");
    _imageWindowAction->setIcon(QIcon(":/icon/svg/imagewindow.svg"));
    _ROIWidowAction = new QAction(tr("ROI Window"), _imageWindowButton);
    _ROIWidowAction->setObjectName("ROIwindow");
    _ROIWidowAction->setIcon(QIcon(":/icon/svg/ROIwindow.svg"));
    _defaultWindowAction = new QAction(tr("Default Window"), this);
    _fullWindowAction = new QAction(tr("Full Window"), this);
    _imageNegativeAction = new QAction(tr("Negative"), this);
    _imageNegativeAction->setIcon(QIcon(":/icon/svg/negative.svg"));

    _zoomAction = new QAction(tr("Zoom Image"), _zoomButton);
    _zoomAction->setObjectName("zoom");
    _zoomAction->setIcon(QIcon(":/icon/svg/zoom.svg"));
    _magnifierAction = new QAction(tr("Magnifier"), _zoomButton);
    _magnifierAction->setObjectName("magnifier");
    _magnifierAction->setIcon(QIcon(":/icon/svg/magnifier.svg"));
    _fitWindowAction = new QAction(tr("Fit Window"), this);
    _fitWindowAction->setIcon(QIcon(":/icon/svg/fitwindow.svg"));
    _1xAction = new QAction("100%", this);
    _2xAction = new QAction("200%", this);
    _4xAction = new QAction("400%", this);
    _8xAction = new QAction("800%", this);
    _zoomInAction = new QAction(tr("Zoom In"), this);
    _zoomInAction->setIcon(QIcon(":/icon/svg/zoomin.svg"));
    _zoomOutAction = new QAction(tr("Zoom Out"), this);
    _zoomOutAction->setIcon(QIcon(":/icon/svg/zoomout.svg"));

    _cursorAction = new QAction(tr("Select"), _cursorButton);
    _cursorAction->setObjectName("cursor");
    _cursorAction->setIcon(QIcon(":/icon/svg/cursor.svg"));
    _moveAction = new QAction(tr("Move"), _cursorButton);
    _moveAction->setObjectName("move");
    _moveAction->setIcon(QIcon(":/icon/svg/move.svg"));

    _rulerAction = new QAction(tr("Length"), _measurementButton);
    _rulerAction->setObjectName("length");
    _rulerAction->setIcon(QIcon(":/icon/svg/length.svg"));
    _arrowAction = new QAction(tr("Arrow"), _measurementButton);
    _arrowAction->setObjectName("arrow");
    _arrowAction->setIcon(QIcon(":/icon/svg/arrow.svg"));
    _angleAction = new QAction(tr("Angle"), _measurementButton);
    _angleAction->setObjectName("angle");
    _angleAction->setIcon(QIcon(":/icon/svg/angle.svg"));
    _plotAction = new QAction(tr("Plot"), _measurementButton);
    _plotAction->setObjectName("plot");
    _plotAction->setIcon(QIcon(":/icon/svg/plot.svg"));
    _imageQualityAction = new QAction(tr("Image Quality"), _measurementButton);
    _imageQualityAction->setObjectName("imagequality");
    _imageQualityAction->setIcon(QIcon(":/icon/svg/imagequality.svg"));
    _rectAction = new QAction(tr("Rectangle"), _measurementButton);
    _rectAction->setObjectName("rectangle");
    _rectAction->setIcon(QIcon(":/icon/svg/rectangle.svg"));
    _roundrectAction = new QAction(tr("Round Rectangle"), _measurementButton);
    _roundrectAction->setObjectName("roundrect");
    _roundrectAction->setIcon(QIcon(":/icon/svg/roundrect.svg"));
    _circleAction = new QAction(tr("Circle"), _measurementButton);
    _circleAction->setObjectName("circle");
    _circleAction->setIcon(QIcon(":/icon/svg/circle.svg"));
    _ellipseAction = new QAction(tr("Ellipse"), _measurementButton);
    _ellipseAction->setObjectName("ellipse");
    _ellipseAction->setIcon(QIcon(":/icon/svg/ellipse.svg"));
    _rhombusAction = new QAction(tr("Rhombus"), _measurementButton);
    _rhombusAction->setObjectName("rhombus");
    _rhombusAction->setIcon(QIcon(":/icon/svg/rhombus.svg"));
    _parallelogramAction = new QAction(tr("Parallelogram"), _measurementButton);
    _parallelogramAction->setObjectName("parallelogram");
    _parallelogramAction->setIcon(QIcon(":/icon/svg/parallelogram.svg"));
    _textAction = new QAction(tr("Text"), _measurementButton);
    _textAction->setObjectName("text");
    _textAction->setIcon(QIcon(":/icon/svg/text.svg"));

    MainWindow* mainWindow = getGlobalWindow();
    connect(_openDicomAction, &QAction::triggered, mainWindow, &MainWindow::openDicomImage);
    connect(_openDicomFolderAction, &QAction::triggered, mainWindow, &MainWindow::openDicomFolderImage);
    connect(_openRawAction, &QAction::triggered, mainWindow, &MainWindow::openRawImage);
    connect(_saveAsRawAction, &QAction::triggered, mainWindow, &MainWindow::saveAsRawImage);

    connect(_showMenuAction, &QAction::toggled, mainWindow, &MainWindow::showMenuBar);
    connect(_showDockWidgetAction, &QAction::toggled, mainWindow, &MainWindow::showDockWidget);
    connect(_singleViewAction, &QAction::triggered, this, &ToolBar::singleViewActionTriggered);
    connect(_threeViewAction, &QAction::triggered, this, &ToolBar::threeViewActionTriggered);
    connect(_fourViewAction, &QAction::triggered, this, &ToolBar::fourViewActionTriggered);
    connect(_volumeViewAction, &QAction::triggered, this, &ToolBar::volumeViewActionTriggered);
    connect(_fullScreenAction, &QAction::triggered, mainWindow, &MainWindow::fullScreen);

    connect(_showAnnotationAction, &QAction::toggled, mainWindow, &MainWindow::showAnnotation);
    connect(_showCrossAction, &QAction::toggled, mainWindow, &MainWindow::showCrossLine);
    connect(_showScaleAction, &QAction::toggled, mainWindow, &MainWindow::showScale);
    connect(_showMeasurementAction, &QAction::toggled, mainWindow, &MainWindow::showMeasurement);

    connect(_flipHorizontalAction, &QAction::triggered, this, &ToolBar::flipHorizontalActionTriggered);
    connect(_flipVerticalAction, &QAction::triggered, this, &ToolBar::flipVerticalActionTriggered);

    connect(_rotate90CWAction, &QAction::triggered, this, &ToolBar::rotate90CWActionTriggered);
    connect(_rotate90CCWAction, &QAction::triggered, this, &ToolBar::rotate90CCWActionTriggered);
    connect(_rotate180Action, &QAction::triggered, this, &ToolBar::rotate180ActionTriggered);
    connect(_resetTransformation, &QAction::triggered, this, &ToolBar::resetTransformationActionTriggered);

    connect(_FPS30Action, &QAction::triggered, this, &ToolBar::FPS30ActionTriggered);
    connect(_FPS60Action, &QAction::triggered, this, &ToolBar::FPS60ActionTriggered);

    connect(_sliceAction, &QAction::triggered, this, &ToolBar::sliceActionTriggered);

    connect(_locationAction, &QAction::triggered, this, &ToolBar::locationActionTriggered);

    connect(_imageWindowAction, &QAction::triggered, this, &ToolBar::imageWindowActionTriggered);
    connect(_ROIWidowAction, &QAction::triggered, this, &ToolBar::ROIWindowActionTriggered);
    connect(_defaultWindowAction, &QAction::triggered, mainWindow->getDocument(), &Document::defaultImageWindow);
    connect(_fullWindowAction, &QAction::triggered, mainWindow->getDocument(), &Document::fullImageWindow);
    connect(_imageNegativeAction, &QAction::triggered, mainWindow->getDocument(), &Document::negativeImage);

    connect(_zoomAction, &QAction::triggered, this, &ToolBar::zoomActionTriggered);
    connect(_magnifierAction, &QAction::triggered, this, &ToolBar::magnifierActionTriggered);
    connect(_fitWindowAction, &QAction::triggered, this, &ToolBar::fitWindowActionTriggered);
    connect(_1xAction, &QAction::triggered, this, &ToolBar::_1xActionTriggered);
    connect(_2xAction, &QAction::triggered, this, &ToolBar::_2xActionTriggered);
    connect(_4xAction, &QAction::triggered, this, &ToolBar::_4xActionTriggered);
    connect(_8xAction, &QAction::triggered, this, &ToolBar::_8xActionTriggered);
    connect(_zoomInAction, &QAction::triggered, mainWindow, &MainWindow::zoomIn);
    connect(_zoomOutAction, &QAction::triggered, mainWindow, &MainWindow::zoomOut);

    connect(_cursorAction, &QAction::triggered, this, &ToolBar::selectItem);
    connect(_moveAction, &QAction::triggered, this, &ToolBar::moveScene);

    connect(_rulerAction, &QAction::triggered, this, &ToolBar::measurementChanged);
    connect(_arrowAction, &QAction::triggered, this, &ToolBar::measurementChanged);
    connect(_angleAction, &QAction::triggered, this, &ToolBar::measurementChanged);
    connect(_plotAction, &QAction::triggered, this, &ToolBar::measurementChanged);
    connect(_imageQualityAction, &QAction::triggered, this, &ToolBar::measurementChanged);
    connect(_rectAction, &QAction::triggered, this, &ToolBar::measurementChanged);
    connect(_roundrectAction, &QAction::triggered, this, &ToolBar::measurementChanged);
    connect(_circleAction, &QAction::triggered, this, &ToolBar::measurementChanged);
    connect(_ellipseAction, &QAction::triggered, this, &ToolBar::measurementChanged);
    connect(_rhombusAction, &QAction::triggered, this, &ToolBar::measurementChanged);
    connect(_parallelogramAction, &QAction::triggered, this, &ToolBar::measurementChanged);
    connect(_textAction, &QAction::triggered, this, &ToolBar::measurementChanged);

    _actionVector.push_back(_sliceAction);
    _actionVector.push_back(_locationAction);
    _actionVector.push_back(_imageWindowAction);
    _actionVector.push_back(_ROIWidowAction);
    _actionVector.push_back(_zoomAction);
    _actionVector.push_back(_magnifierAction);
    _actionVector.push_back(_moveAction);
    _actionVector.push_back(_cursorAction);
    _actionVector.push_back(_rulerAction);
    _actionVector.push_back(_arrowAction);
    _actionVector.push_back(_angleAction);
    _actionVector.push_back(_plotAction);
    _actionVector.push_back(_imageQualityAction);
    _actionVector.push_back(_rectAction);
    _actionVector.push_back(_roundrectAction);
    _actionVector.push_back(_circleAction);
    _actionVector.push_back(_ellipseAction);
    _actionVector.push_back(_rhombusAction);
    _actionVector.push_back(_parallelogramAction);
    _actionVector.push_back(_textAction);
}

void ToolBar::initButton()
{
    MainWindow* mainWindow = getGlobalWindow();
    QMenu* menu = new QMenu(this);
    menu->addAction(_openDicomAction);
    menu->addAction(_openDicomFolderAction);
    menu->addSeparator();
    menu->addAction(_openRawAction);
    _openButton->setMenu(menu);
    _openButton->setIcon(QIcon(":/icon/svg/open.svg"));
    _openButton->setToolTip(tr("Open Image File"));
    connect(_openButton, &QToolButton::clicked, mainWindow, &MainWindow::openImage);

    menu = new QMenu(this);
    menu->addAction(_saveAsRawAction);
    _saveButton->setMenu(menu);
    _saveButton->setIcon(QIcon(":/icon/svg/save.svg"));
    _saveButton->setToolTip(tr("Save Image File"));
    connect(_saveButton, &QToolButton::clicked, mainWindow, &MainWindow::saveAs);

    _printButton->setIcon(QIcon(":/icon/svg/print.svg"));
    _printButton->setToolTip(tr("Print"));
    connect(_printButton, &QToolButton::clicked, mainWindow, &MainWindow::print);

    menu = new QMenu(this);
    menu->addAction(_showMenuAction);
    menu->addAction(_showDockWidgetAction);
    menu->addSeparator();
    menu->addAction(_singleViewAction);
    menu->addAction(_threeViewAction);
    menu->addAction(_fourViewAction);
    menu->addAction(_volumeViewAction);
    menu->addSeparator();
    menu->addAction(_fullScreenAction);
    _layoutButton->setMenu(menu);
    _layoutButton->setIcon(QIcon(":/icon/svg/layout.svg"));
    _layoutButton->setToolTip(tr("Change Layout"));
    connect(_layoutButton, &QToolButton::clicked, this, &ToolBar::layoutButtonClicked);

    menu = new QMenu(this);
    menu->addAction(_showAnnotationAction);
    menu->addAction(_showCrossAction);
    menu->addAction(_showScaleAction);
    menu->addAction(_showMeasurementAction);
    _showInfoButton->setMenu(menu);
    _showInfoButton->setIcon(QIcon(":/icon/svg/annotation.svg"));
    _showInfoButton->setToolTip(tr("Toggle Annotations"));
    connect(_showInfoButton, &QToolButton::clicked, this, &ToolBar::showInfoButtonClicked);

    menu = new QMenu(this);
    menu->addAction(_flipHorizontalAction);
    menu->addAction(_flipVerticalAction);
    _flipButton->setMenu(menu);
    _flipButton->setIcon(QIcon(":/icon/svg/flip_horizontal.svg"));
    _flipButton->setToolTip(tr("Flip"));
    connect(_flipButton, &QToolButton::clicked, _flipHorizontalAction, &QAction::triggered);

    menu = new QMenu(this);
    menu->addAction(_rotate90CWAction);
    menu->addAction(_rotate90CCWAction);
    menu->addSeparator();
    menu->addAction(_rotate180Action);
    menu->addSeparator();
    menu->addAction(_resetTransformation);
    _rotateButton->setMenu(menu);
    _rotateButton->setIcon(QIcon(":/icon/svg/rotate_cw.svg"));
    _rotateButton->setToolTip(tr("Rotate"));
    connect(_rotateButton, &QToolButton::clicked, _rotate90CWAction, &QAction::triggered);

    menu = new QMenu(this);
    menu->addAction(_FPS30Action);
    menu->addAction(_FPS60Action);
    _cineButton->setMenu(menu);
    _cineButton->setIcon(QIcon(":/icon/svg/cine.svg"));
    _cineButton->setToolTip(tr("Cine"));
    connect(_cineButton, &QToolButton::clicked, _FPS30Action, &QAction::triggered);

    _sliceButton->setIconByName(":/icon/svg/slice.svg");
    _sliceButton->setToolTip(tr("Browse slices"));
    _sliceButton->installEventFilter(this);
    _sliceButton->setCurrentAction(_sliceAction);

    _locationButton->setIconByName(":/icon/svg/location.svg");
    _locationButton->setToolTip(tr("Location"));
    _locationButton->installEventFilter(this);
    _locationButton->setCurrentAction(_locationAction);

    menu = new QMenu(this);
    menu->addAction(_imageWindowAction);
    menu->addAction(_ROIWidowAction);
    menu->addSeparator();
    menu->addAction(_defaultWindowAction);
    menu->addAction(_fullWindowAction);
    menu->addSeparator();
    menu->addAction(_imageNegativeAction);
    _imageWindowButton->setMenu(menu);
    _imageWindowButton->setIconByName(":/icon/svg/imagewindow.svg");
    _imageWindowButton->setToolTip(tr("Adjust Image Window"));
    _imageWindowButton->installEventFilter(this);
    _imageWindowButton->setCurrentAction(_imageWindowAction);
    connect(_imageWindowButton, &QToolButton::triggered, this, &ToolBar::imageWindowButtonTriggered);

    _undoButton->setIcon(QIcon(":/icon/svg/undo.svg"));
    _undoButton->setToolTip(tr("Undo"));
    connect(_undoButton, &QToolButton::clicked, mainWindow->getDocument(), &Document::undo);

    _restoreButton->setIcon(QIcon(":/icon/svg/restore.svg"));
    _restoreButton->setToolTip(tr("Restore"));
    connect(_restoreButton, &QToolButton::clicked, mainWindow->getDocument(), &Document::restore);

    menu = new QMenu(this);
    menu->addAction(_zoomAction);
    menu->addAction(_magnifierAction);
    menu->addSeparator();
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
    _zoomButton->setIconByName(":/icon/svg/zoom.svg");
    _zoomButton->setToolTip(tr("Zoom Image"));
    _zoomButton->installEventFilter(this);
    _zoomButton->setCurrentAction(_zoomAction);
    connect(_zoomButton, &QToolButton::triggered, this, &ToolBar::zoomButtonTriggered);

    menu = new QMenu(this);
    menu->addAction(_cursorAction);
    menu->addAction(_moveAction);
    _cursorButton->setMenu(menu);
    _cursorButton->setIconByName(":/icon/svg/cursor.svg");
    _cursorButton->setToolTip(tr("Select Item/Move Image"));
    _cursorButton->installEventFilter(this);
    _cursorButton->setCurrentAction(_cursorAction);
    connect(_cursorButton, &QToolButton::triggered, this, &ToolBar::cursorButtonTriggered);

    menu = new QMenu(this);
    menu->addAction(_rulerAction);
    menu->addAction(_arrowAction);
    menu->addAction(_angleAction);
    menu->addAction(_plotAction);
    menu->addAction(_imageQualityAction);
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
    _measurementButton->setIconByName(":/icon/svg/length.svg");
    _measurementButton->setToolTip(tr("Measurements and Tools"));
    _measurementButton->installEventFilter(this);
    _measurementButton->setCurrentAction(_rulerAction);
    connect(_measurementButton, &QToolButton::triggered, this, &ToolBar::measurementButtonTriggered);
}

void ToolBar::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        _openDicomAction->setText(tr("Open &DICOM File..."));
        _openDicomFolderAction->setText(tr("Open DICOM &Folder..."));
        _openRawAction->setText(tr("Open &Raw File..."));
        _saveAsRawAction->setText(tr("Save As &Raw File..."));

        _showMenuAction->setText(tr("Menu"));
        _showDockWidgetAction->setText(tr("Dock Widgets"));
        _singleViewAction->setText(tr("Single View"));
        _threeViewAction->setText(tr("Three View"));
        _fourViewAction->setText(tr("Four View"));
        _volumeViewAction->setText(tr("Volume View"));
        _fullScreenAction->setText(tr("Full Screen Mode"));
        _showAnnotationAction->setText(tr("Annotations"));
        _showCrossAction->setText(tr("Cross Reference Line"));
        _showScaleAction->setText(tr("Image Scale"));
        _showMeasurementAction->setText(tr("Measurements"));

        _flipHorizontalAction->setText(tr("Flip Horizontal"));
        _flipVerticalAction->setText(tr("Flip Vertical"));

        _rotate90CWAction->setText(tr("Rotate 90 ") + QString(QChar(0x00B0)) + tr(" CW"));
        _rotate90CCWAction->setText(tr("Rotate 90 ") + QString(QChar(0x00B0)) + tr(" CCW"));
        _rotate180Action->setText(tr("Rotate 180 ") + QString(QChar(0x00B0)));
        _resetTransformation->setText(tr("Reset Transformation"));

        _sliceAction->setText(tr("Browse slices"));

        _locationAction->setText(tr("Location"));

        _imageWindowAction->setText(tr("Adjust Window"));
        _ROIWidowAction->setText(tr("ROI Window"));
        _defaultWindowAction->setText(tr("Default Window"));
        _fullWindowAction->setText(tr("Full Window"));
        _imageNegativeAction->setText(tr("Negative"));

        _zoomAction->setText(tr("Zoom Image"));
        _magnifierAction->setText(tr("Magnifier"));
        _fitWindowAction->setText(tr("Fit Window"));
        _zoomInAction->setText(tr("Zoom In"));
        _zoomOutAction->setText(tr("Zoom Out"));

        _cursorAction->setText(tr("Select"));
        _moveAction->setText(tr("Move"));

        _rulerAction->setText(tr("Length"));
        _arrowAction->setText(tr("Arrow"));
        _angleAction->setText(tr("Angle"));
        _plotAction->setText(tr("Plot"));
        _imageQualityAction->setText(tr("Image Quality"));
        _rectAction->setText(tr("Rectangle"));
        _roundrectAction->setText(tr("Round Rectangle"));
        _circleAction->setText(tr("Circle"));
        _ellipseAction->setText(tr("Ellipse"));
        _rhombusAction->setText(tr("Rhombus"));
        _parallelogramAction->setText(tr("Parallelogram"));
        _textAction->setText(tr("Text"));

        _openButton->setToolTip(tr("Open Image File"));
        _saveButton->setToolTip(tr("Save Image File"));
        _printButton->setToolTip(tr("Print"));
        _layoutButton->setToolTip(tr("Change Layout"));
        _showInfoButton->setToolTip(tr("Toggle Annotations"));
        _flipButton->setToolTip(tr("Flip"));
        _rotateButton->setToolTip(tr("Rotate"));
        _cineButton->setToolTip(tr("Cine"));
        _sliceButton->setToolTip(tr("Browse slices"));
        _locationButton->setToolTip(tr("Location"));
        _imageWindowButton->setToolTip(tr("Adjust Image Window"));
        _zoomButton->setToolTip(tr("Zoom Image"));
        _cursorButton->setToolTip(tr("Select Item/Move Image"));
        _measurementButton->setToolTip(tr("Measurements and Tools"));
        _undoButton->setToolTip(tr("Undo"));
        _restoreButton->setToolTip(tr("Restore"));
    }

    QToolBar::changeEvent(event);
}

bool ToolBar::eventFilter(QObject* obj, QEvent* event)
{
    ToolButton* toolButton = static_cast<ToolButton*>(obj);
    if (toolButton && toolButton->isEnabled())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
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
    case DiagramItem::Plot:
        _plotAction->trigger();
        _measurementButton->triggered(_plotAction);
        break;
    case DiagramItem::ImageQuality:
        _imageQualityAction->trigger();
        _measurementButton->triggered(_imageQualityAction);
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

void ToolBar::layoutButtonClicked()
{
    bool checked = !_showMenuAction->isChecked();

    _showMenuAction->setChecked(checked);
    _showDockWidgetAction->setChecked(checked);

    _showMenuAction->toggled(checked);
    _showDockWidgetAction->toggled(checked);
}

void ToolBar::showInfoButtonClicked()
{
    bool checked = !_showAnnotationAction->isChecked();

    _showAnnotationAction->setChecked(checked);
    _showScaleAction->setChecked(checked);
    _showMeasurementAction->setChecked(checked);
}

void ToolBar::singleViewActionTriggered()
{
    getGlobalWindow()->singleView();
}

void ToolBar::threeViewActionTriggered()
{
    getGlobalWindow()->threeView();
}

void ToolBar::fourViewActionTriggered()
{
    getGlobalWindow()->fourView();
}

void ToolBar::volumeViewActionTriggered()
{
    getGlobalWindow()->volumeView();
}

void ToolBar::flipHorizontalActionTriggered()
{
    getGlobalActiveView()->flipHorizontal();
}

void ToolBar::flipVerticalActionTriggered()
{
    getGlobalActiveView()->flipVertical();
}

void ToolBar::rotate90CWActionTriggered()
{
    getGlobalActiveView()->rotate90CW();
}

void ToolBar::rotate90CCWActionTriggered()
{
    getGlobalActiveView()->rotate90CCW();
}

void ToolBar::rotate180ActionTriggered()
{
    getGlobalActiveView()->rotate180();
}

void ToolBar::resetTransformationActionTriggered()
{
    getGlobalActiveView()->resetTransformation();
}

void ToolBar::FPS30ActionTriggered()
{
    _FPS30Action->setChecked(true);
    _FPS60Action->setChecked(false);
    bool result = getGlobalWindow()->cine30FPS();
    _cineButton->setChecked(result);
}

void ToolBar::FPS60ActionTriggered()
{
    _FPS30Action->setChecked(false);
    _FPS60Action->setChecked(true);
    bool result = getGlobalWindow()->cine60FPS();
    _cineButton->setChecked(result);
}

void ToolBar::sliceActionTriggered()
{
    _sliceButton->setIconByName(":/icon/svg/slice.svg");
    _sliceButton->setMouseHandler(new SliceMouseHandler());
}

void ToolBar::locationActionTriggered()
{
    _locationButton->setIconByName(":/icon/svg/location.svg");
    _locationButton->setMouseHandler(new LocationMouseHandler());
}

void ToolBar::imageWindowActionTriggered()
{
    _imageWindowButton->setIconByName(":/icon/svg/imagewindow.svg");
    _imageWindowButton->setMouseHandler(new ImageWindowMouseHandler());
}

void ToolBar::ROIWindowActionTriggered()
{
    _imageWindowButton->setIconByName(":/icon/svg/ROIwindow.svg");
    _imageWindowButton->setMouseHandler(new ROIWindowMouseHandler());
}

void ToolBar::imageWindowButtonTriggered(QAction* action)
{
    if (action == _imageWindowAction || action == _ROIWidowAction)
    {
        _imageWindowButton->activeAction(action);
    }
}

void ToolBar::zoomActionTriggered()
{
    _zoomButton->setIconByName(":/icon/svg/zoom.svg");
    _zoomButton->setMouseHandler(new ZoomMouseHandler());
}

void ToolBar::magnifierActionTriggered()
{
    _zoomButton->setIconByName(":/icon/svg/magnifier.svg");
    _zoomButton->setMouseHandler(new MagnifierMouseHandler());
}

void ToolBar::fitWindowActionTriggered()
{
    if (getGlobalWindow()->isViewLinked())
    {
        getGlobalAxialView()->fitWindow();
        getGlobalCoronalView()->fitWindow();
        getGlobalSagittalView()->fitWindow();
    }
    else
    {
        getGlobalActiveView()->fitWindow();
    }
}

void ToolBar::_1xActionTriggered()
{
    if (getGlobalWindow()->isViewLinked())
    {
        getGlobalAxialView()->zoomNormal();
        getGlobalCoronalView()->zoomNormal();
        getGlobalSagittalView()->zoomNormal();
    }
    else
    {
        getGlobalActiveView()->zoomNormal();
    }
}

void ToolBar::_2xActionTriggered()
{
    if (getGlobalWindow()->isViewLinked())
    {
        getGlobalAxialView()->zoom2x();
        getGlobalCoronalView()->zoom2x();
        getGlobalSagittalView()->zoom2x();
    }
    else
    {
        getGlobalActiveView()->zoom2x();
    }
}

void ToolBar::_4xActionTriggered()
{
    if (getGlobalWindow()->isViewLinked())
    {
        getGlobalAxialView()->zoom4x();
        getGlobalCoronalView()->zoom4x();
        getGlobalSagittalView()->zoom4x();
    }
    else
    {
        getGlobalActiveView()->zoom4x();
    }
}

void ToolBar::_8xActionTriggered()
{
    if (getGlobalWindow()->isViewLinked())
    {
        getGlobalAxialView()->zoom8x();
        getGlobalCoronalView()->zoom8x();
        getGlobalSagittalView()->zoom8x();
    }
    else
    {
        getGlobalActiveView()->zoom8x();
    }
}

void ToolBar::zoomButtonTriggered(QAction* action)
{
    if (action == _zoomAction || action == _magnifierAction)
    {
        _zoomButton->activeAction(action);
    }
}

void ToolBar::selectItem()
{
    getGlobalAxialView()->setSceneMode(MOVE_ITEM);
    getGlobalCoronalView()->setSceneMode(MOVE_ITEM);
    getGlobalSagittalView()->setSceneMode(MOVE_ITEM);

    _cursorButton->setIconByName(":/icon/svg/cursor.svg");
    _cursorButton->setMouseHandler(new SelectMouseHandler());
}

void ToolBar::moveScene()
{
    getGlobalAxialView()->setSceneMode(MOVE_SCENE);
    getGlobalCoronalView()->setSceneMode(MOVE_SCENE);
    getGlobalSagittalView()->setSceneMode(MOVE_SCENE);

    _cursorButton->setIconByName(":/icon/svg/move.svg");
    _cursorButton->setMouseHandler(new MoveMouseHandler());
}

void ToolBar::cursorButtonTriggered(QAction* action)
{
    _cursorButton->activeAction(action);
}

void ToolBar::measurementChanged()
{
    MainWindow* mainWindow = getGlobalWindow();
    mainWindow->setToolBoxWidgetVisible(true, false);

    QAction* action = qobject_cast<QAction*>(sender());
    if (action == _rulerAction)
    {
        _measurementButton->setIconByName(":/icon/svg/length.svg");
        getGlobalAxialView()->setItemType(DiagramItem::Line);
        getGlobalCoronalView()->setItemType(DiagramItem::Line);
        getGlobalSagittalView()->setItemType(DiagramItem::Line);
    }
    else if (action == _arrowAction)
    {
        _measurementButton->setIconByName(":/icon/svg/arrow.svg");
        getGlobalAxialView()->setItemType(DiagramItem::Arrow);
        getGlobalCoronalView()->setItemType(DiagramItem::Arrow);
        getGlobalSagittalView()->setItemType(DiagramItem::Arrow);
    }
    else if (action == _angleAction)
    {
        _measurementButton->setIconByName(":/icon/svg/angle.svg");
        getGlobalAxialView()->setItemType(DiagramItem::Angle);
        getGlobalCoronalView()->setItemType(DiagramItem::Angle);
        getGlobalSagittalView()->setItemType(DiagramItem::Angle);
    }
    else if (action == _plotAction)
    {
        _measurementButton->setIconByName(":/icon/svg/plot.svg");
        getGlobalAxialView()->setItemType(DiagramItem::Plot);
        getGlobalCoronalView()->setItemType(DiagramItem::Plot);
        getGlobalSagittalView()->setItemType(DiagramItem::Plot);
    }
    else if (action == _imageQualityAction)
    {
        _measurementButton->setIconByName(":/icon/svg/imagequality.svg");
        getGlobalAxialView()->setItemType(DiagramItem::ImageQuality);
        getGlobalCoronalView()->setItemType(DiagramItem::ImageQuality);
        getGlobalSagittalView()->setItemType(DiagramItem::ImageQuality);
    }
    else if (action == _rectAction)
    {
        _measurementButton->setIconByName(":/icon/svg/rectangle.svg");
        getGlobalAxialView()->setItemType(DiagramItem::Rect);
        getGlobalCoronalView()->setItemType(DiagramItem::Rect);
        getGlobalSagittalView()->setItemType(DiagramItem::Rect);
    }
    else if (action == _roundrectAction)
    {
        _measurementButton->setIconByName(":/icon/svg/roundrect.svg");
        getGlobalAxialView()->setItemType(DiagramItem::RoundRect);
        getGlobalCoronalView()->setItemType(DiagramItem::RoundRect);
        getGlobalSagittalView()->setItemType(DiagramItem::RoundRect);
    }
    else if (action == _circleAction)
    {
        _measurementButton->setIconByName(":/icon/svg/circle.svg");
        getGlobalAxialView()->setItemType(DiagramItem::Circle);
        getGlobalCoronalView()->setItemType(DiagramItem::Circle);
        getGlobalSagittalView()->setItemType(DiagramItem::Circle);
    }
    else if (action == _ellipseAction)
    {
        _measurementButton->setIconByName(":/icon/svg/ellipse.svg");
        getGlobalAxialView()->setItemType(DiagramItem::Ellipse);
        getGlobalCoronalView()->setItemType(DiagramItem::Ellipse);
        getGlobalSagittalView()->setItemType(DiagramItem::Ellipse);
    }
    else if (action == _rhombusAction)
    {
        _measurementButton->setIconByName(":/icon/svg/rhombus.svg");
        getGlobalAxialView()->setItemType(DiagramItem::Rhombus);
        getGlobalCoronalView()->setItemType(DiagramItem::Rhombus);
        getGlobalSagittalView()->setItemType(DiagramItem::Rhombus);
    }
    else if (action == _parallelogramAction)
    {
        _measurementButton->setIconByName(":/icon/svg/parallelogram.svg");
        getGlobalAxialView()->setItemType(DiagramItem::Parallelogram);
        getGlobalCoronalView()->setItemType(DiagramItem::Parallelogram);
        getGlobalSagittalView()->setItemType(DiagramItem::Parallelogram);
    }
    else if (action == _textAction)
    {
        _measurementButton->setIconByName(":/icon/svg/text.svg");
        getGlobalAxialView()->setItemType(DiagramItem::Text);
        getGlobalCoronalView()->setItemType(DiagramItem::Text);
        getGlobalSagittalView()->setItemType(DiagramItem::Text);
        mainWindow->setToolBoxWidgetVisible(false, true);
    }
    getGlobalAxialView()->setSceneMode(INSERT_ITEM);
    getGlobalCoronalView()->setSceneMode(INSERT_ITEM);
    getGlobalSagittalView()->setSceneMode(INSERT_ITEM);
    _measurementButton->setMouseHandler(new DrawMouseHandler());
}

void ToolBar::measurementButtonTriggered(QAction* action)
{
    _measurementButton->activeAction(action);
}
