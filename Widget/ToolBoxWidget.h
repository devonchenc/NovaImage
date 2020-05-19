#pragma once

#include "BaseWidget.h"
#include "DiagramItem.h"

QT_BEGIN_NAMESPACE
class QButtonGroup;
class QColorButton;
class QGridLayout;
class QFrame;
class QLabel;
class QFontComboBox;
class QComboBox;
class QToolButton;
QT_END_NAMESPACE

class ToolBoxWidget : public BaseWidget
{
	Q_OBJECT

public:
	ToolBoxWidget(QWidget* parent = nullptr);
	virtual ~ToolBoxWidget();

public:
	virtual void init();

	virtual void reset();

signals:
	void setSceneMode(int mode);
	void setItemType(DiagramItem::DiagramType type);

	void setLineColor(const QColor& color);
	void setFillColor(const QColor& color);
	void setTextColor(const QColor& color);
	void setTextFont(const QFont& font);

public slots:
	void buttonGroupClicked(int id);

	//	void textButtonTriggered();
	void lineColorButtonTriggered();
	void fillColorButtonTriggered();
	void textColorButtonTriggered();

	void itemInserted(QGraphicsItem* item);
	void itemSelected(QGraphicsItem* item);
	void textSelected(QGraphicsItem* item);

	void currentFontChanged(const QFont& font);
	void fontSizeChanged(const QString& size);

	void handleFontChange();

protected:
	void changeEvent(QEvent* event) override;

private:
	void initUI();

	QGridLayout* createToolButton();

	QWidget* createCellWidget(const QString &text, DiagramItem::DiagramType type);

	void setWidgetVisible(bool line, bool text);

	QLabel* getLabel(QWidget* widget);

private:
	QButtonGroup* _buttonGroup;

	QWidget* pointWidget;
	QWidget* rectangle;
	QWidget* roundRect;
	QWidget* circle;
	QWidget* ellipse;
	QWidget* rhombus;
	QWidget* parallelogram;
	QWidget* textWidget;
	QWidget* lineWidget;

	QFrame* _lineFrame;
	QLabel* _labelLine;
	QLabel* _labelFill;
	QColorButton* _buttonLineColor;
	QColorButton* _buttonFillColor;
	QLabel* _labelFont;
	QLabel* _labelSize;
	QLabel* _labelStyle;
	QFontComboBox* _fontCombo;
	QComboBox* _fontSizeCombo;
	QToolButton* _buttonBold;
	QToolButton* _buttonItalic;
	QToolButton* _buttonUnderline;
	QLabel* _labelText;
	QColorButton* _buttonTextColor;
};