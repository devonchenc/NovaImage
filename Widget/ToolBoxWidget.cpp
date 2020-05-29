#include "ToolBoxWidget.h"

#include <QGridLayout>
#include <QIcon>
#include <QToolButton>
#include <QButtonGroup>
#include <QFrame>
#include <QLabel>
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
	_labelFill = new QLabel(tr("Fill:"));
	_buttonLineColor = new QColorButton(Qt::blue);
	connect(_buttonLineColor, &QColorButton::clicked, this, &ToolBoxWidget::lineColorButtonTriggered);
	_buttonFillColor = new QColorButton(Qt::white);
	connect(_buttonFillColor, &QColorButton::clicked, this, &ToolBoxWidget::fillColorButtonTriggered);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addWidget(_labelLine);
	hLayout->addWidget(_buttonLineColor);
	hLayout->addStretch();
	hLayout->addWidget(_labelFill);
	hLayout->addWidget(_buttonFillColor);
	hLayout->addStretch();

	_labelFont = new QLabel("Font:");
	_fontCombo = new QFontComboBox();
	connect(_fontCombo, &QFontComboBox::currentFontChanged, this, &ToolBoxWidget::currentFontChanged);
	_fontCombo->setEditable(false);
	QHBoxLayout* h2Layout = new QHBoxLayout;
	h2Layout->addWidget(_labelFont);
	h2Layout->addWidget(_fontCombo);
	h2Layout->addStretch();

	_labelSize = new QLabel("Size:");
	_fontSizeCombo = new QComboBox;
	_fontSizeCombo->setEditable(true);
	for (int i = 8; i <= 80; i = i + 4)
		_fontSizeCombo->addItem(QString().setNum(i));
	QIntValidator* validator = new QIntValidator(2, 64, this);
	_fontSizeCombo->setValidator(validator);
	connect(_fontSizeCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(fontSizeChanged(QString)));
	QHBoxLayout* h3Layout = new QHBoxLayout;
	h3Layout->addWidget(_labelSize);
	h3Layout->addWidget(_fontSizeCombo);
	h3Layout->addStretch();

	_labelStyle = new QLabel("Style:");
	_buttonBold = new QToolButton;
	_buttonBold->setCheckable(true);
	_buttonBold->setIcon(QIcon(QPixmap("Resources/bold.png")));
	connect(_buttonBold, SIGNAL(clicked()), this, SLOT(handleFontChange()));
	_buttonItalic = new QToolButton;
	_buttonItalic->setCheckable(true);
	_buttonItalic->setIcon(QIcon(QPixmap("Resources/italic.png")));
	connect(_buttonItalic, SIGNAL(clicked()), this, SLOT(handleFontChange()));
	_buttonUnderline = new QToolButton;
	_buttonUnderline->setCheckable(true);
	_buttonUnderline->setIcon(QIcon(QPixmap("Resources/underline.png")));
	connect(_buttonUnderline, SIGNAL(clicked()), this, SLOT(handleFontChange()));
	h3Layout->addWidget(_labelStyle);
	h3Layout->addWidget(_buttonBold);
	h3Layout->addWidget(_buttonItalic);
	h3Layout->addWidget(_buttonUnderline);
	h3Layout->addStretch();

	_labelText = new QLabel("Color:");
	_buttonTextColor = new QColorButton(Qt::green);
	connect(_buttonTextColor, SIGNAL(clicked()), this, SLOT(textColorButtonTriggered()));
	QHBoxLayout* h4Layout = new QHBoxLayout;
	h4Layout->addWidget(_labelText);
	h4Layout->addWidget(_buttonTextColor);
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

		getLabel(pointWidget)->setText(tr("Point"));
		getLabel(rectangle)->setText(tr("Rectangle"));
		getLabel(roundRect)->setText(tr("RoundRect"));
		getLabel(circle)->setText(tr("Circle"));
		getLabel(ellipse)->setText(tr("Ellipse"));
		getLabel(rhombus)->setText(tr("Rhombus"));
		getLabel(parallelogram)->setText(tr("Parallelogram"));
		getLabel(textWidget)->setText(tr("Text"));
		getLabel(lineWidget)->setText(tr("Line"));

		_labelLine->setText(tr("Line:"));
		_labelFill->setText(tr("Fill:"));
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
	pointWidget = new QWidget;
	pointWidget->setLayout(pointLayout);
	gridLayout->addWidget(pointWidget, 0, 0);

	rectangle = createCellWidget(tr("Rectangle"), DiagramItem::Rect);
	roundRect = createCellWidget(tr("RoundRect"), DiagramItem::RoundRect);
	circle = createCellWidget(tr("Circle"), DiagramItem::Circle);
	ellipse = createCellWidget(tr("Ellipse"), DiagramItem::Ellipse);
	rhombus = createCellWidget(tr("Rhombus"), DiagramItem::Rhombus);
	parallelogram = createCellWidget(tr("Parallelogram"), DiagramItem::Parallelogram);
	gridLayout->addWidget(rectangle, 0, 1);
	gridLayout->addWidget(roundRect, 0, 2);
	gridLayout->addWidget(circle, 0, 3);
	gridLayout->addWidget(ellipse, 0, 4);
	gridLayout->addWidget(rhombus, 1, 0);
	gridLayout->addWidget(parallelogram, 1, 1);

	QToolButton* textButton = new QToolButton;
	textButton->setCheckable(true);
	textButton->setIconSize(iconSize);
	textButton->setIcon(QIcon(QPixmap("Resources/textpointer.png")));
	_buttonGroup->addButton(textButton, DiagramItem::Text);
	QGridLayout* textLayout = new QGridLayout;
	textLayout->addWidget(textButton, 0, 0, Qt::AlignHCenter);
	textLayout->addWidget(new QLabel(tr("Text")), 1, 0, Qt::AlignCenter);
	textWidget = new QWidget;
	textWidget->setLayout(textLayout);
	gridLayout->addWidget(textWidget, 1, 2);

	QToolButton* lineButton = new QToolButton;
	lineButton->setCheckable(true);
	lineButton->setIconSize(iconSize);
	lineButton->setIcon(QIcon(QPixmap("Resources/line.png")));
	_buttonGroup->addButton(lineButton, DiagramItem::Line);
	QGridLayout* lineLayout = new QGridLayout;
	lineLayout->addWidget(lineButton, 0, 0, Qt::AlignHCenter);
	lineLayout->addWidget(new QLabel(tr("Line")), 1, 0, Qt::AlignCenter);
	lineWidget = new QWidget;
	lineWidget->setLayout(lineLayout);
	gridLayout->addWidget(lineWidget, 1, 3);

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
	_labelFill->setVisible(line);
	_buttonLineColor->setVisible(line);
	_buttonFillColor->setVisible(line);

	_labelFont->setVisible(text);
	_fontCombo->setVisible(text);
	_labelSize->setVisible(text);
	_fontSizeCombo->setVisible(text);
	_labelStyle->setVisible(text);
	_buttonBold->setVisible(text);
	_buttonItalic->setVisible(text);
	_buttonUnderline->setVisible(text);
	_labelText->setVisible(text);
	_buttonTextColor->setVisible(text);
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
	QColor color = _buttonLineColor->getColor();
	emit setLineColor(color);
}

void ToolBoxWidget::fillColorButtonTriggered()
{
	QColor color = _buttonFillColor->getColor();
	emit setFillColor(color);
}

void ToolBoxWidget::textColorButtonTriggered()
{
	QColor color = _buttonTextColor->getColor();
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
	emit setSceneMode(MOVE_ITEM);
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
	_buttonLineColor->setColor(lineColor);
	_buttonFillColor->setColor(fillColor);
}

void ToolBoxWidget::textSelected(QGraphicsItem* item)
{
	DiagramTextItem* textItem = qgraphicsitem_cast<DiagramTextItem*>(item);
	Q_ASSERT(textItem);

	setWidgetVisible(false, true);

	QFont font = textItem->font();
//	_fontCombo->setCurrentFont(font);
	_fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
	_buttonBold->setChecked(font.weight() == QFont::Bold);
	_buttonItalic->setChecked(font.italic());
	_buttonUnderline->setChecked(font.underline());

	QColor color = textItem->defaultTextColor();
	_buttonTextColor->setColor(color);
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
	font.setWeight(_buttonBold->isChecked() ? QFont::Bold : QFont::Normal);
	font.setItalic(_buttonItalic->isChecked());
	font.setUnderline(_buttonUnderline->isChecked());

	emit setTextFont(font);
}