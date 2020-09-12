#pragma once

#include "BaseProcessor.h"

class InverseProcessor : public BaseProcessor
{
public:
    InverseProcessor(QObject* parent = nullptr);
    virtual ~InverseProcessor();

protected:
    void processImageImpl(GeneralImage* image, QImage* dstImage) override;

    void processImageImpl(MonoImage* image, QImage* dstImage) override;

    // Process float array
    void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;
};
