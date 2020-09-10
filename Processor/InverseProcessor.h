#pragma once

#include "BaseProcessor.h"

class InverseProcessor : public BaseProcessor
{
public:
    InverseProcessor(QObject* parent = nullptr);
    virtual ~InverseProcessor();

protected:
    void processGeneralImage(GeneralImage* image) override;

    void processMonoImage(MonoImage* image) override;

    // Process float array
    void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;
};
