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
    void init() override;

    void reset() override;
    // Levels adjust
    void levelsAdjust(float bottom, float mid, float top);

    void setParameters(float bottom, float mid, float top);

    LevelsProcessor* getProcessor() { return _processor; }

public slots:
    void resetButtonClicked();

    void apply();

    void updateHistogram();

    void updateBottom(float bottom);

    void updateMid(float mid);

    void updateTop(float top);

protected:
    void changeEvent(QEvent* event) override;

private:
    HistogramWidget* _histogram;
    QLineEdit* _editMin;
    QLineEdit* _editMid;
    QLineEdit* _editMax;

    // Image processor
    LevelsProcessor* _processor;
};
