#pragma once

#include <QSlider>
#include <QLabel>

#include "BaseWidget.h"

class CommonProcessor;

class CommonWidget : public BaseWidget
{
    Q_OBJECT

public:
    CommonWidget(QWidget* parent = nullptr);
    virtual ~CommonWidget();

public:
    void init() override;

    void reset() override;

public slots:
    void brightValueChanged(int value);

    void contrastValueChanged(int value);

protected:
    void changeEvent(QEvent* event) override;

private:
    void SetBrightnessAndContrast();

private:
    QSlider* _sliderBright;
    QSlider* _sliderContrast;
    QLabel* _labelBrightNum;
    QLabel* _labelContrastNum;

    QLabel* _labelBright;
    QLabel* _labelContrast;

    // Image processor
    CommonProcessor* _processor;
};
