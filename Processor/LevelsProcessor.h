#pragma once

#include "BaseProcessor.h"

class LevelsProcessor : public BaseProcessor
{
public:
    LevelsProcessor(QObject* parent = nullptr);
    virtual ~LevelsProcessor();

protected:
    void processImage(GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(MonoImage* srcImage, MonoImage* dstImage) override;

    void processByteImage(uchar* byteImage, int width, int height);

    // Process float array
    virtual void processArray(float* array, int width, int height, float fMinValue, float fMaxValue, uchar* pByte) override;

public:
    void setPara(float bottom, float mid, float top);

private:
    // Pointer to levels widget
    float _bottom, _mid, _top;
};
