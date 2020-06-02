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
#include "../GraphicsScene.h"
#include "DiagramLineItem.h"

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

	_labelLine = new QLabel(tr("Line:"));
	_lineColorButton = new QColorButton(Qt::blue);
	connect(_lineColorButton, &QColorButton::clicked, this, &ToolBoxWidget::lineColorButtonTriggered);
	_fillCheckBox = new QCheckBox(tr("Fill:"));
	connect(_fillCheckBox, &QCheckBox::stateChanged, this, &ToolBoxWidget::enableFillColor);
	_fillColorButton = new QColorButton(Qt::white);
	connect(_fillColorButton, &QColorButton::clicked, this, &ToolBoxWidget::fillColorButtonTriggered);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addWidget(_labelLine);
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
	connect(_boldButton, SIGNAL(clicked()), this, SLOT(handleFontChange()));
	_italicButton = new QToolButton;
	_italicButton->setCheckable(true);
	_italicButton->setIcon(QIcon(QPixmap("Resources/italic.png")));
	connect(_italicButton, SIGNAL(clicked()), this, SLOT(handleFontChange()));
	_underlineButton = new QToolButton;
	_underlineButton->setCheckable(true);
	_underlineButton->setIcon(QIcon(QPixmap("Resources/underline.png")));
	connect(_underlineButton, SIGNAL(clicked()), this, SLOT(handleFontChange()));
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

		getLabel(_pointWidget)->setText(tr("Point"));
		getLabel(_rectangleWidget)->setText(tr("Rectangle"));
		getLabel(_roundRectWidget)->setText(tr("RoundRect"));
		getLabel(_circleWidget)->setText(tr("Circle"));
		getLabel(_ellipseWidget)->setText(tr("Ellipse"));
		getLabel(_rhombusWidget)->setText(tr("Rhombus"));
		getLabel(_parallelogramWidget)->setText(tr("Parallelogram"));
		getLabel(_textWidget)->setText(tr("Text"));
		getLabel(_lineWidget)->setText(tr("Line"));

		_labelLine->setText(tr("Line:"));
		_fillCheckBox->setText(tr("Fill:"));
	}

	BaseWidget::changeEvent(event);
}

QLabel* ToolBoxWidget::getLabel(QWidget* widget)
{
	QLayout* layout = widget->layout();
	for (int i = 0; i < layout->count(); i++)
	{
		QLayoutItem* item = layout->itemAt(i);
		QLabel* label = qobject_cast<QLabel*>(item->widget());
		if (label)
		{
			return label;
		}
	}
	return nullptr;
}

QGridLayout* ToolBoxWidget::createToolButton()
{
	QGridLayout* gridLayout = new QGridLayout;

	QToolButton* pointButton = new QToolButton;
	pointButton->setCheckable(true);
	pointButton->setIconSize(iconSize);
	pointButton->setIcon(QIcon(QPixmap("Resources/pointer.png")));
	_buttonGroup->addButton(pointButton, DiagramItem::None);
	QGridLayout* pointLayout = new QGridLayout;
	pointLayout->addWidget(pointButton, 0, 0, Qt::AlignHCenter);
	pointLayout->addWidget(new QLabel(tr("Point")), 1, 0, Qt::AlignCenter);
	_pointWidget = new QWidget;
	_pointWidget->setLayout(pointLayout);
	gridLayout->addWidget(_pointWidget, 0, 0);

	_rectangleWidget = createCellWidget(tr("Rectangle"), DiagramItem::Rect);
	_roundRectWidget = createCellWidget(tr("RoundRect"), DiagramItem::RoundRect);
	_circleWidget = createCellWidget(tr("Circle"), DiagramItem::Circle);
	_ellipseWidget = createCellWidget(tr("Ellipse"), DiagramItem::Ellipse);
	_rhombusWidget = createCellWidget(tr("Rhombus"), DiagramItem::Rhombus);
	_parallelogramWidget = createCellWidget(tr("Parallelogram"), DiagramItem::Parallelogram);
	gridLayout->addWidget(_rectangleWidget, 0, 1);
	gridLayout->addWidget(_roundRectWidget, 0, 2);
	gridLayout->addWidget(_circleWidget, 0, 3);
	gridLayout->addWidget(_ellipseWidget, 0, 4);
	gridLayout->addWidget(_rhombusWidget, 1, 0);
	gridLayout->addWidget(_parallelogramWidget, 1, 1);

	QToolButton* textButton = new QToolButton;
	textButton->setCheckable(true);
	textButton->setIconSize(iconSize);
	textButton->setIcon(QIcon(QPixmap("Resources/textpointer.png")));
	_buttonGroup->addButton(textButton, DiagramItem::Text);
	QGridLayout* textLayout = new QGridLayout;
	textLayout->addWidget(textButton, 0, 0, Qt::AlignHCenter);
	textLayout->addWidget(new QLabel(tr("Text")), 1, 0, Qt::AlignCenter);
	_textWidget = new QWidget;
	_textWidget->setLayout(textLayout);
	gridLayout->addWidget(_textWidget, 1, 2);

	QToolButton* lineButton = new QToolButton;
	lineButton->setCheckable(true);
	lineButton->setIconSize(iconSize);
	lineButton->setIcon(QIcon(QPixmap("Resources/line.png")));
	_buttonGroup->addButton(lineButton, DiagramItem::Line);
	QGridLayout* lineLayout = new QGridLayout;
	lineLayout->addWidget(lineButton, 0, 0, Qt::AlignHCenter);
	lineLayout->addWidget(new QLabel(tr("Line")), 1, 0, Qt::AlignCenter);
	_lineWidget = new QWidget;
	_lineWidget->setLayout(lineLayout);
	gridLayout->addWidget(_lineWidget, 1, 3);

	return gridLayout;
}

QWidget* ToolBoxWidget::createCellWidget(const QString& text, DiagramItem::DiagramType type)
{
	DiagramItem item(type);
	QIcon icon(item.image());

	QToolButton* button = new QToolButton;
	button->setIcon(icon);
	button->setIconSize(iconSize);
	button->setCheckable(true);
	_buttonGroup->addButton(button, int(type));

	QGridLayout* layout = new QGridLayout;
	layout->addWidget(button, 0, 0, Qt::AlignHCenter);
	layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

	QWidget* widget = new QWidget;
	widget->setLayout(layout);

	return widget;
}

void ToolBoxWidget::setWidgetVisible(bool line, bool text)
{
	_lineFrame->setVisible(line || text);

	_labelLine->setVisible(line);
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
		emit setSceneMode(MOVE_ITEM);
		return;
	}

	if (id == DiagramItem::None)
	{
		setWidgetVisible(false, false);
		emit setSceneMode(SELECT_ITEM);
	}
	else
	{
		if (id == DiagramItem::Text)
			setWidgetVisible(false, true);
		else
			setWidgetVisible(true, false);
		emit setSceneMode(INSERT_ITEM);
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
//	emit setSceneMode(MOVE_ITEM);
}

void ToolBoxWidget::itemSelected(QGraphicsItem* item)
{
	QColor lineColor, fillColor;
	if (item->type() == DiagramItem::Type)
	{
		DiagramItem* diagramitem = qgraphicsitem_cast<DiagramItem*>(item);
		lineColor = diagramitem->pen().color();
		fillColor = diagramitem->brush().color();
		setWidgetVisible(true, false);
	}
	else if (item->type() == DiagramLineItem::Type)
	{
		DiagramLineItem* lineitem = qgraphicsitem_cast<DiagramLineItem*>(item);
		lineColor = lineitem->pen().color();
		fillColor = lineitem->pointPen().color();
		setWidgetVisible(true, false);
	}
	_lineColorButton->setColor(lineColor);
	_fillColorButton->setColor(fillColor);
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