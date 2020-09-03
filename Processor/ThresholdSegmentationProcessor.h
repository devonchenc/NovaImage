#pragma once

#include <QSlider>
#include <QLabel>

#include "BaseProcessor.h"

class ThresholdSegmentationProcessor : public BaseProcessor
{
public:
    ThresholdSegmentationProcessor();
    virtual ~ThresholdSegmentationProcessor();

    QWidget* initUI() override;

protected:
    void processGeneralImage(GeneralImage* image) override;

    void processMonoImage(MonoImage* image) override;

    // Process float array
    void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;

private:
    int findOtsuThreshold(uint* grayPixelArray, int pixelNumber);

private:
    QLabel* _thresholdLabel;
    QSlider* _thresholdSlider;
};
