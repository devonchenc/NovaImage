#pragma once

#include "BaseWidget.h"

class LevelsProcessor;
class HistogramWidget;
QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class LevelsWidget : public BaseWidget
{
	Q_OBJECT

public:
	LevelsWidget(QWidget* parent = nullptr);
	virtual ~LevelsWidget();

public:
	virtual void init();

	virtual void reset();

public slots:
	void updateHistogram();

	void updateBottom(float bottom);

	void updateMid(float mid);

	void updateTop(float top);

protected:
	void changeEvent(QEvent* event) override;

private:
	// Levels adjust
	void levelsAdjust();

private:
	HistogramWidget* _histogram;
	QLineEdit* _editMin;
	QLineEdit* _editMid;
	QLineEdit* _editMax;

	// Image processor
	LevelsProcessor* _processor;
};