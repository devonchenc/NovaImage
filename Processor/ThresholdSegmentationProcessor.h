#pragma once

#include <QSlider>
#include <QLabel>

#include "BaseProcessor.h"

class ThresholdSegmentationWidget : public QWidget
{
public:
    ThresholdSegmentationWidget(QWidget* parent = nullptr);

public slots:
    void valueChanged(int value);

private:
    QLabel* _thresholdLabel;
    QSlider* _thresholdSlider;
    QLabel* _thresholdValueLabel;
};

class ThresholdSegmentationProcessor : public BaseProcessor
{
public:
    ThresholdSegmentationProcessor();
    virtual ~ThresholdSegmentationProcessor();

protected:
    void processGeneralImage(GeneralImage* image) override;

    void processMonoImage(MonoImage* image) override;

    // Process float array
    void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;

private:
    int findOtsuThreshold(uint* grayPixelArray, int pixelNumber);

private:
    ThresholdSegmentationWidget* _widget;
};
