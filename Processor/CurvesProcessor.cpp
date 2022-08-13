#include "CurvesProcessor.h"

#include <cmath>
#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"

CurvesProcessor::CurvesProcessor(QObject* parent)
    : BaseProcessor(false, parent)
{

}

void CurvesProcessor::processImage(GeneralImage* srcImage, GeneralImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width = srcImage->width();
    int height = srcImage->height();
    QImage* imageEntity = srcImage->getImageEntity();
    uchar* srcData = imageEntity->bits();
    uchar* dstData = dstImage->getImageEntity()->bits();
    int pitch = imageEntity->bytesPerLine();
    int depth = imageEntity->depth() / 8;

    float variable = 255.0f / float(_arrayNum - 1);

    if (_channel == CURVE_CHANNEL_GRAY)
    {
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width * depth; i++)
            {
                uchar* dstPixel = dstData + j * pitch + i;
                uchar* srcPixel = srcData + j * pitch + i;
                *(dstPixel) = interpolation(*srcPixel, _arrayIntensity, _arrayNum, variable);
            }
        }
    }
    else
    {
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                uchar* dstPixel = dstData + j * pitch + i * depth;
                uchar* srcPixel = srcData + j * pitch + i * depth;
                *(dstPixel) = interpolation(*srcPixel, _arrayBlue, _arrayNum, variable);
                *(dstPixel + 1) = interpolation(*(srcPixel + 1), _arrayGreen, _arrayNum, variable);
                *(dstPixel + 2) = interpolation(*(srcPixel + 2), _arrayRed, _arrayNum, variable);
            }
        }
    }
}

void CurvesProcessor::processImage(MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    float maxValue = srcImage->getMaxValue();
    float minValue = srcImage->getMinValue();
    if (maxValue == minValue)
        return;

    int width, height;
    uchar* byteImage = dstImage->getBYTEImage(width, height);

    if (_channel == CURVE_CHANNEL_GRAY)
    {
        float fVariable1 = float(_arrayNum - 1) / (maxValue - minValue);
        float fVariable2 = (maxValue - minValue) / float(_arrayNum - 1);
        float fVariable3 = 255.0f / (maxValue - minValue);
        for (int i = 0; i < width * height; i++)
        {
            float srcValue = srcImage->getValue(i);
            float dstValue = interpolation2(srcValue - minValue, _arrayIntensity, _arrayNum, fVariable1, fVariable2) + minValue;
            dstImage->setValue(i, dstValue);

            uchar value = uchar((dstValue - minValue) * fVariable3);
            byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = value;
        }
    }
    else
    {
        float fVariable1 = (maxValue - minValue) / (_arrayNum - 1);
        float fVariable2 = 255.0f / (_arrayNum - 1);
        for (int i = 0; i < width * height; i++)
        {
            byteImage[3 * i] = interpolation(srcImage->getValue(i) - minValue, _arrayRed, _arrayNum, fVariable1, fVariable2);
            byteImage[3 * i + 1] = interpolation(srcImage->getValue(i) - minValue, _arrayGreen, _arrayNum, fVariable1, fVariable2);
            byteImage[3 * i + 2] = interpolation(srcImage->getValue(i) - minValue, _arrayBlue, _arrayNum, fVariable1, fVariable2);
        }
    }

    dstImage->copyByteToImage();
}

void CurvesProcessor::setArray(int arrayNum, uint* arrayIntensity, uint* arrayRed, uint* arrayGreen, uint* arrayBlue)
{
    _arrayNum = arrayNum;

    _arrayIntensity = arrayIntensity;
    _arrayRed = arrayRed;
    _arrayGreen = arrayGreen;
    _arrayBlue = arrayBlue;
}

// Interpolation
uchar CurvesProcessor::interpolation(uchar target, uint* array, int arrayNum, float variable)
{
    if (variable == 0)
        return array[0];

    float percentage = target / variable;
    int integer = int(percentage);
    float fraction = percentage - integer;
    if (integer == arrayNum - 1)
    {
        return uchar(round(array[integer] * variable));
    }
    else
    {
        return uchar(round((array[integer] * (1.0f - fraction) + array[integer + 1] * fraction) * variable));
    }
}

template <typename Type>
uchar CurvesProcessor::interpolation(Type target, uint* array, int arrayNum, float variable1, float variable2)
{
    if (variable1 == 0)
        return array[0];

    float percentage = float(target) / variable1;
    int integer = int(percentage);
    float fraction = percentage - integer;
    if (integer == arrayNum - 1)
    {
        return (uchar(round(array[integer] * variable2)));
    }
    else
    {
        return (uchar(round((array[integer] * (1.0f - fraction) + array[integer + 1] * fraction) * variable2)));
    }
}

float CurvesProcessor::interpolation2(float src, uint* array, int arrayNum, float variable1, float variable2)
{
    if (variable1 == 0)
        return 0;

    float percentage = src * variable1;
    int integer = int(percentage);
    float fraction = percentage - integer;
    if (integer == arrayNum - 1)
    {
        return array[integer] * variable2;
    }
    else
    {
        return (array[integer] * (1.0f - fraction) + array[integer + 1] * fraction) * variable2;
    }
}
