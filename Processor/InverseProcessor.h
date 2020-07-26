#pragma once

#include "BaseProcessor.h"

class InverseProcessor : public BaseProcessor
{
public:
    InverseProcessor();
    virtual ~InverseProcessor();

protected:
    void processGeneralImage(GeneralImage* image) override;

    void processMonoImage(MonoImage* image) override;

    // Process float array
    virtual void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;
};
