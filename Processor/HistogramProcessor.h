#pragma once

#include "BaseProcessor.h"

class HistogramProcessor : public BaseProcessor
{
public:
    HistogramProcessor(QObject* parent = nullptr);
    virtual ~HistogramProcessor();

protected:
    void processImage(GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(MonoImage* srcImage, MonoImage* dstImage) override;

    // Process float array
    void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;

public:
    void setWindowArray(bool* array, int arrayNum);

private:
    bool* _array;
    int _arrayNum;
};
