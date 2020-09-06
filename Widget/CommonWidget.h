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
    void setBrightnessAndContrast();

private:
    QSlider* _brightSlider;
    QSlider* _contrastSlider;
    QLabel* _brightValueLabel;
    QLabel* _contrastValueLabel;

    QLabel* _brightLabel;
    QLabel* _contrastLabel;

    // Image processor
    CommonProcessor* _processor;
};
