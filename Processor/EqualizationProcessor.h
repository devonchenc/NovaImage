#pragma once

#include "BaseProcessor.h"

class EqualizationProcessor : public BaseProcessor
{
public:
    EqualizationProcessor();
    virtual ~EqualizationProcessor();

protected:
    void processGeneralImage(GeneralImage* image) override;

    void processMonoImage(MonoImage* image) override;

    // Process float array
    void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;

private:
    void RGB2HSV(uchar R, uchar G, uchar B, int& H, float& S, uchar& V);
    void HSV2RGB(int H, float S, uchar V, uchar& R, uchar& G, uchar& B);
};
