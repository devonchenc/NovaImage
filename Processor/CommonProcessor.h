#pragma once

#include "BaseProcessor.h"

class CommonProcessor : public BaseProcessor
{
public:
    CommonProcessor(QObject* parent = nullptr);
    virtual ~CommonProcessor();

protected:
    void processImageImpl(GeneralImage* image, QImage* dstImage) override;

    void processImageImpl(MonoImage* image, QImage* dstImage) override;

    // Process float array
    void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;

public:
    void setBrightnessAndContrast(int brightness, int contrast);

private:
    int _brightness;
    int _contrast;
};
