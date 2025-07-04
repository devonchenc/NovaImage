#pragma once

#include "BaseWidget.h"
#include "../Diagram/DiagramItem.h"

QT_BEGIN_NAMESPACE
class QButtonGroup;
class ColorButton;
class QGridLayout;
class QFrame;
class QLabel;
class QCheckBox;
class QFontComboBox;
class QComboBox;
class QToolButton;
class QSlider;
QT_END_NAMESPACE

class ToolBoxWidget : public BaseWidget
{
    Q_OBJECT

public:
    ToolBoxWidget(QWidget* parent = nullptr);

public:
    void init() override;

    void reset() override;

signals:
    void setItemType(DiagramItem::DiagramType type);

    void setLineColor(const QColor& color);
    void setEnableFillColor(bool enable);
    void setFillColor(const QColor& color);
    void setTransparency(int value);
    void setTextColor(const QColor& color);
    void setTextFont(const QFont& font);

public slots:
    void buttonGroupClicked(int id);

    void lineColorButtonTriggered();
    void enableFillColor(int state);
    void fillColorButtonTriggered();
    void textColorButtonTriggered();

    void itemInserted(QGraphicsItem* item);
    void itemSelected(QGraphicsItem* item);
    void textSelected(QGraphicsItem* item);

    void transparencyValueChanged(int value);

    void currentFontChanged(const QFont& font);
    void fontSizeChanged(const QString& size);

    void handleFontChange();

    void setWidgetVisible(bool line, bool text);

protected:
    void changeEvent(QEvent* event) override;

private:
    void initUI();

    QGridLayout* createToolButton();

    QWidget* createCellWidget(const QString& text, DiagramItem::DiagramType type);

    QToolButton* getToolButton(QWidget* widget);

private:
    QButtonGroup* _buttonGroup;

    QWidget* _cursorWidget;
    QWidget* _lengthWidget;
    QWidget* _arrowWidget;
    QWidget* _angleWidget;
    QWidget* _plotWidget;
    QWidget* _rectangleWidget;
    QWidget* _roundRectWidget;
    QWidget* _circleWidget;
    QWidget* _ellipseWidget;
    QWidget* _rhombusWidget;
    QWidget* _parallelogramWidget;
    QWidget* _textWidget;

    QFrame* _lineFrame;
    QLabel* _lineLabel;
    QCheckBox* _fillCheckBox;
    ColorButton* _lineColorButton;
    ColorButton* _fillColorButton;
    QLabel* _transparencyLabel;
    QSlider* _transparencySlider;
    QLabel* _transparencyValueLabel;

    QLabel* _fontLabel;
    QLabel* _sizeLabel;
    QLabel* _styleLabel;
    QFontComboBox* _fontCombo;
    QComboBox* _fontSizeCombo;
    QToolButton* _boldButton;
    QToolButton* _italicButton;
    QToolButton* _underlineButton;
    QLabel* _textLabel;
    ColorButton* _textColorButton;
};
