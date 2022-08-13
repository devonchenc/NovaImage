#pragma once

#include "BaseWidget.h"
#include "CurveSquare.h"

class CurvesProcessor;

QT_BEGIN_NAMESPACE
class QLabel;
class QRadioButton;
class QTextStream;
class QComboBox;
class QPushButton;
QT_END_NAMESPACE

class CurvesWidget : public BaseWidget
{
    Q_OBJECT

public:
    CurvesWidget(QWidget* parent = nullptr);

public:
    void init() override;

    void reset() override;

public slots:
    void channelChanged(int value);

    void resetButtonClicked();

    void reverseButtonClicked();

    void applyButtonClicked();

    void curveRadioToggled();
    void linearRadioToggled();

    void clickSave();

    void clickLoad();

protected slots:
    void resizeSquare();

    void updateImage();

    void updateLabelText(QString input, QString output);

protected:
    void changeEvent(QEvent* event) override;

private:
    void generateHistogram();

    void connectSqureWithProcessor();

    void savePegArray(QTextStream& stream, const PegArray& pegs);

    void loadPegArray(QTextStream& stream, PegArray& pegs);

private:
    QLabel* _channelLabel;
    QLabel* _inputLabel;
    QLabel* _inputValueLabel;
    QLabel* _outputLabel;
    QLabel* _outputValueLabel;

    QComboBox* _channelComboBox;
    QRadioButton* _curveRadio;
    QRadioButton* _linearRadio;

    QPushButton* _resetButton;
    QPushButton* _reverseButton;
    QPushButton* _saveButton;
    QPushButton* _loadButton;

    QPushButton* _applyButton;

    CurveSquare* _square;

    // Image processor
    CurvesProcessor* _processor;
};
