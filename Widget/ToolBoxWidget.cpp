#include "ToolBoxWidget.h"

#include <QGridLayout>
#include <QIcon>
#include <QToolButton>
#include <QButtonGroup>
#include <QFrame>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QButtonGroup>
#include <QEvent>

#include "ColorButton.h"
#include "../Core/GraphicsScene.h"
#include "DiagramLineItem.h"
#include "DiagramAngleItem.h"

const QSize iconSize = QSize(32, 32);

ToolBoxWidget::ToolBoxWidget(QWidget* parent)
	: BaseWidget(parent)
{
	setName(tr("ToolBox"));

	_buttonGroup = new QButtonGroup(this);
	_buttonGroup->setExclusive(false);
	connect(_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupClicked(int)));
	initUI();
}

ToolBoxWidget::~ToolBoxWidget()
{
	
}

void ToolBoxWidget::init()
{

}

void ToolBoxWidget::reset()
{

}

void ToolBoxWidget::initUI()
{
	_lineFrame = new QFrame;
	_lineFrame->setFrameShape(QFrame::HLine);
	_lineFrame->setFrameShadow(QFrame::Sunken);

	_lineLabel = new QLabel(tr("Line:"));
	_lineColorButton = new QColorButton(qRgb(0, 255, 55));
	connect(_lineColorButton, &QColorButton::clicked, this, &ToolBoxWidget::lineColorButtonTriggered);
	_fillCheckBox = new QCheckBox(tr("Fill:"));
	connect(_fillCheckBox, &QCheckBox::stateChanged, this, &ToolBoxWidget::enableFillColor);
	_fillColorButton = new QColorButton(Qt::white);
	connect(_fillColorButton, &QColorButton::clicked, this, &ToolBoxWidget::fillColorButtonTriggered);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addWidget(_lineLabel);
	hLayout->addWidget(_lineColorButton);
	hLayout->addStretch();
	hLayout->addWidget(_fillCheckBox);
	hLayout->addWidget(_fillColorButton);
	hLayout->addStretch();

	_fontLabel = new QLabel("Font:");
	_fontCombo = new QFontComboBox();
	connect(_fontCombo, &QFontComboBox::currentFontChanged, this, &ToolBoxWidget::currentFontChanged);
	_fontCombo->setEditable(false);
	QHBoxLayout* h2Layout = new QHBoxLayout;
	h2Layout->addWidget(_fontLabel);
	h2Layout->addWidget(_fontCombo);
	h2Layout->addStretch();

	_sizeLabel = new QLabel("Size:");
	_fontSizeCombo = new QComboBox;
	_fontSizeCombo->setEditable(true);
	for (int i = 8; i <= 80; i = i + 4)
		_fontSizeCombo->addItem(QString().setNum(i));
	QIntValidator* validator = new QIntValidator(2, 64, this);
	_fontSizeCombo->setValidator(validator);
	connect(_fontSizeCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(fontSizeChanged(QString)));
	QHBoxLayout* h3Layout = new QHBoxLayout;
	h3Layout->addWidget(_sizeLabel);
	h3Layout->addWidget(_fontSizeCombo);
	h3Layout->addStretch();

	_styleLabel = new QLabel("Style:");
	_boldButton = new QToolButton;
	_boldButton->setCheckable(true);
	_boldButton->setIcon(QIcon(QPixmap("Resources/bold.png")));
	connect(_boldButton, &QToolButton::clicked, this, &ToolBoxWidget::handleFontChange);
	_italicButton = new QToolButton;
	_italicButton->setCheckable(true);
	_italicButton->setIcon(QIcon(QPixmap("Resources/italic.png")));
	connect(_italicButton, &QToolButton::clicked, this, &ToolBoxWidget::handleFontChange);
	_underlineButton = new QToolButton;
	_underlineButton->setCheckable(true);
	_underlineButton->setIcon(QIcon(QPixmap("Resources/underline.png")));
	connect(_underlineButton, &QToolButton::clicked, this, &ToolBoxWidget::handleFontChange);
	h3Layout->addWidget(_styleLabel);
	h3Layout->addWidget(_boldButton);
	h3Layout->addWidget(_italicButton);
	h3Layout->addWidget(_underlineButton);
	h3Layout->addStretch();

	_textLabel = new QLabel("Color:");
	_textColorButton = new QColorButton(Qt::green);
	connect(_textColorButton, SIGNAL(clicked()), this, SLOT(textColorButtonTriggered()));
	QHBoxLayout* h4Layout = new QHBoxLayout;
	h4Layout->addWidget(_textLabel);
	h4Layout->addWidget(_textColorButton);
	h4Layout->addStretch();

	QGridLayout* gridLayout = createToolButton();
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addLayout(gridLayout);
	vLayout->addWidget(_lineFrame);
	vLayout->addLayout(hLayout);
	vLayout->addLayout(h2Layout);
	vLayout->addLayout(h3Layout);
	vLayout->addLayout(h4Layout);
	vLayout->addStretch();

	setLayout(vLayout);

	setWidgetVisible(false, false);
}

void ToolBoxWidget::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		parentWidget()->setWindowTitle(tr("ToolBox"));

		getToolButton(_cursorWidget)->setToolTip(tr("Point"));
		getToolButton(_lengthWidget)->setToolTip(tr("Line"));
		getToolButton(_angleWidget)->setToolTip(tr("Angle"));
		getToolButton(_arrowWidget)->setToolTip(tr("Arrow"));
		getToolButton(_rectangleWidget)->setToolTip(tr("Rectangle"));
		getToolButton(_roundRectWidget)->setToolTip(tr("RoundRect"));
		getToolButton(_circleWidget)->setToolTip(tr("Circle"));
		getToolButton(_ellipseWidget)->setToolTip(tr("Ellipse"));
		getToolButton(_rhombusWidget)->setToolTip(tr("Rhombus"));
		getToolButton(_parallelogramWidget)->setToolTip(tr("Parallelogram"));
		getToolButton(_textWidget)->setToolTip(tr("Text"));

		_lineLabel->setText(tr("Line:"));
		_fillCheckBox->setText(tr("Fill:"));
	}

	BaseWidget::changeEvent(event);
}

QToolButton* ToolBoxWidget::getToolButton(QWidget* widget)
{
	QLayout* layout = widget->layout();
	for (int i = 0; i < layout->count(); i++)
	{
		QLayoutItem* item = layout->itemAt(i);
		QToolButton* toolButton = qobject_cast<QToolButton*>(item->widget());
		if (toolButton)
		{
			return toolButton;
		}
	}
	return nullptr;
}

QGridLayout* ToolBoxWidget::createToolButton()
{
	_cursorWidget = createCellWidget(tr("Cursor"), DiagramItem::None);
	_lengthWidget = createCellWidget(tr("Length"), DiagramItem::Line);
	_angleWidget = createCellWidget(tr("Angle"), DiagramItem::Angle);
	_arrowWidget = createCellWidget(tr("Arrow"), DiagramItem::Arrow);
	_rectangleWidget = createCellWidget(tr("Rectangle"), DiagramItem::Rect);
	_roundRectWidget = createCellWidget(tr("RoundRect"), DiagramItem::RoundRect);
	_circleWidget = createCellWidget(tr("Circle"), DiagramItem::Circle);
	_ellipseWidget = createCellWidget(tr("Ellipse"), DiagramItem::Ellipse);
	_rhombusWidget = createCellWidget(tr("Rhombus"), DiagramItem::Rhombus);
	_parallelogramWidget = createCellWidget(tr("Parallelogram"), DiagramItem::Parallelogram);
	_textWidget = createCellWidget(tr("Text"), DiagramItem::Text);

	QGridLayout* gridLayout = new QGridLayout;
	gridLayout->addWidget(_cursorWidget, 0, 0);
	gridLayout->addWidget(_lengthWidget, 0, 1);
	gridLayout->addWidget(_angleWidget, 0, 2);
	gridLayout->addWidget(_arrowWidget, 0, 3);
	gridLayout->addWidget(_rectangleWidget, 0, 4);
	gridLayout->addWidget(_roundRectWidget, 1, 0);
	gridLayout->addWidget(_circleWidget, 1, 1);
	gridLayout->addWidget(_ellipseWidget, 1, 2);
	gridLayout->addWidget(_rhombusWidget, 1, 3);
	gridLayout->addWidget(_parallelogramWidget, 1, 4);
	gridLayout->addWidget(_textWidget, 2, 0);

	return gridLayout;
}

QWidget* ToolBoxWidget::createCellWidget(const QString& text, DiagramItem::DiagramType type)
{
	QString str = QString(":/icon/svg/%1.svg").arg(text);
	QIcon icon(str.toLower());

	QToolButton* button = new QToolButton;
	button->setIcon(icon);
	button->setIconSize(iconSize);
	button->setCheckable(true);
	button->setToolTip(text);
	_buttonGroup->addButton(button, int(type));

	QGridLayout* layout = new QGridLayout;
	layout->addWidget(button, 0, 0, Qt::AlignHCenter);
//	layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

	QWidget* widget = new QWidget;
	widget->setLayout(layout);

	return widget;
}

void ToolBoxWidget::setWidgetVisible(bool line, bool text)
{
	_lineFrame->setVisible(line || text);

	_lineLabel->setVisible(line);
	_fillCheckBox->setVisible(line);
	_lineColorButton->setVisible(line);
	_fillColorButton->setVisible(line);

	_fontLabel->setVisible(text);
	_fontCombo->setVisible(text);
	_sizeLabel->setVisible(text);
	_fontSizeCombo->setVisible(text);
	_styleLabel->setVisible(text);
	_boldButton->setVisible(text);
	_italicButton->setVisible(text);
	_underlineButton->setVisible(text);
	_textLabel->setVisible(text);
	_textColorButton->setVisible(text);
}

void ToolBoxWidget::buttonGroupClicked(int id)
{
	QList<QAbstractButton*> buttons = _buttonGroup->buttons();
	QAbstractButton* clickedButton = _buttonGroup->button(id);

	// set other button unchecked
	foreach (QAbstractButton* button, buttons)
	{
		if (clickedButton != button)
			button->setChecked(false);
	}

	// simply set objButton unchecked if already checked
	if (!clickedButton->isChecked())
	{
		emit setItemType(DiagramItem::None);
		return;
	}

	if (id == DiagramItem::None)
	{
		setWidgetVisible(false, false);
		emit setItemType(DiagramItem::None);
	}
	else
	{
		if (id == DiagramItem::Text)
			setWidgetVisible(false, true);
		else
			setWidgetVisible(true, false);
		emit setItemType(DiagramItem::DiagramType(id));
	}
}

void ToolBoxWidget::lineColorButtonTriggered()
{
	QColor color = _lineColorButton->getColor();
	emit setLineColor(color);
}

void ToolBoxWidget::enableFillColor(int state)
{
	emit setEnableFillColor(state == Qt::Checked);
}

void ToolBoxWidget::fillColorButtonTriggered()
{
	QColor color = _fillColorButton->getColor();
	emit setFillColor(color);
}

void ToolBoxWidget::textColorButtonTriggered()
{
	QColor color = _textColorButton->getColor();
	emit setTextColor(color);
}

void ToolBoxWidget::itemInserted(QGraphicsItem* item)
{
	if (item->type() == DiagramItem::Type)
	{
		DiagramItem* diagramitem = qgraphicsitem_cast<DiagramItem*>(item);
		_buttonGroup->button(int(diagramitem->diagramType()))->setChecked(false);
	}
	else if (item->type() == DiagramTextItem::Type)
	{
		_buttonGroup->button(int(DiagramItem::Text))->setChecked(false);
	}
	else if (item->type() == DiagramLineItem::Type)
	{
		_buttonGroup->button(int(DiagramItem::Line))->setChecked(false);
	}
	else if (item->type() == DiagramAngleItem::Type)
	{
		_buttonGroup->button(int(DiagramItem::Angle))->setChecked(false);
	}
}

void ToolBoxWidget::itemSelected(QGraphicsItem* item)
{
	QColor lineColor, fillColor;
	bool hasBrush;
	if (item->type() == DiagramItem::Type)
	{
		DiagramItem* diagramitem = qgraphicsitem_cast<DiagramItem*>(item);
		lineColor = diagramitem->pen().color();
		fillColor = diagramitem->brush().color();
		hasBrush = diagramitem->brush().style() != Qt::NoBrush ? true : false;
		setWidgetVisible(true, false);
	}
	else if (item->type() == DiagramLineItem::Type)
	{
		DiagramLineItem* lineitem = qgraphicsitem_cast<DiagramLineItem*>(item);
		lineColor = lineitem->pen().color();
		fillColor = lineitem->pointPen().color();
		hasBrush = false;
		setWidgetVisible(true, false);
	}
	_lineColorButton->setColor(lineColor);
	_fillColorButton->setColor(fillColor);
	_fillCheckBox->setChecked(hasBrush);
}

void ToolBoxWidget::textSelected(QGraphicsItem* item)
{
	DiagramTextItem* textItem = qgraphicsitem_cast<DiagramTextItem*>(item);
	Q_ASSERT(textItem);

	setWidgetVisible(false, true);

	QFont font = textItem->font();
//	_fontCombo->setCurrentFont(font);
	_fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
	_boldButton->setChecked(font.weight() == QFont::Bold);
	_italicButton->setChecked(font.italic());
	_underlineButton->setChecked(font.underline());

	QColor color = textItem->defaultTextColor();
	_textColorButton->setColor(color);
}

void ToolBoxWidget::currentFontChanged(const QFont& font)
{
	handleFontChange();
}

void ToolBoxWidget::fontSizeChanged(const QString& size)
{
	handleFontChange();
}

void ToolBoxWidget::handleFontChange()
{
	QFont font = _fontCombo->currentFont();
	font.setPointSize(_fontSizeCombo->currentText().toInt());
	font.setWeight(_boldButton->isChecked() ? QFont::Bold : QFont::Normal);
	font.setItalic(_italicButton->isChecked());
	font.setUnderline(_underlineButton->isChecked());

	emit setTextFont(font);
}