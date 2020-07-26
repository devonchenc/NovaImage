#pragma once

#include "BaseProcessor.h"

class CommonProcessor : public BaseProcessor
{
public:
    CommonProcessor();
    virtual ~CommonProcessor();

protected:
    void processGeneralImage(GeneralImage* image) override;

    void processMonoImage(MonoImage* image) override;

    // Process float array
    void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;

public:
    void setBrightnessAndContrast(int brightness, int contrast);

private:
    int _brightness;
    int _contrast;
};
