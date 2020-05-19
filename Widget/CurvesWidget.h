#pragma once

#include <QLabel>
#include <QRadioButton>

#include "BaseWidget.h"
#include "CurveSquare.h"

class CurvesProcessor;

QT_BEGIN_NAMESPACE
class QTextStream;
class QComboBox;
class QPushButton;
QT_END_NAMESPACE

class CurvesWidget : public BaseWidget
{
	Q_OBJECT

public:
	CurvesWidget(QWidget* parent = nullptr);
	virtual ~CurvesWidget();

public:
	virtual void init();

	virtual void reset();

public slots:
	void channelChanged(int value);

	void clickReset();

	void clickReverse();

	void toggleCurveRadio();
	void toggleLinearRadio();

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
	QLabel* _labelChannel;
	QLabel* _labelInput;
	QLabel* _labelInputValue;
	QLabel* _labelOutput;
	QLabel* _labelOutputValue;

	QComboBox* _comboboxChannel;
	QRadioButton* _radioCurve;
	QRadioButton* _radioLinear;

	QPushButton* _buttonReset;
	QPushButton* _buttonReverse;
	QPushButton* _buttonSave;
	QPushButton* _buttonLoad;

	CurveSquare* _square;

	// Image processor
	CurvesProcessor* _processor;
};