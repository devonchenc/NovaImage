#pragma once

#include "BaseProcessor.h"

#ifndef CURVE_CHANNEL
#define CURVE_CHANNEL
#define CURVE_CHANNEL_GRAY      0
#define CURVE_CHANNEL_RED       1
#define CURVE_CHANNEL_GREEN     2
#define CURVE_CHANNEL_BLUE      3
#endif

class CurvesProcessor : public BaseProcessor
{
public:
    CurvesProcessor(QObject* parent = nullptr);

protected:
    void processImage(GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(MonoImage* srcImage, MonoImage* dstImage) override;

public:
    void setChannel(int channel) { _channel = channel; }

    void setArray(int arrayNum, uint* arrayIntensity, uint* arrayRed, uint* arrayGreen, uint* arrayBlue);

private:
    // Interpolation
    uchar interpolation(uchar target, uint* array, int arrayNum, float variable);

    template <typename Type>
    uchar interpolation(Type target, uint* array, int arrayNum, float variable1, float variable2);

    float interpolation2(float src, uint* array, int arrayNum, float variable1, float variable2);

private:
    int _channel;

    int _arrayNum;
    uint* _arrayIntensity;
    uint* _arrayRed;
    uint* _arrayGreen;
    uint* _arrayBlue;
};
