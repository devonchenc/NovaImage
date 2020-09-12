#include "CurvesProcessor.h"

#include <cmath>
#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"

CurvesProcessor::CurvesProcessor(QObject* parent)
    : BaseProcessor(false, parent)
{

}

CurvesProcessor::~CurvesProcessor()
{

}

void CurvesProcessor::processImageImpl(GeneralImage* image, QImage* dstImage)
{
    assert(image);

    int width = image->width();
    int height = image->height();
    QImage* imageEntity = image->getImageEntity();
    uchar* imageData = imageEntity->bits();
    uchar* dstData = dstImage->bits();
    int pitch = imageEntity->bytesPerLine();
    int depth = imageEntity->depth() / 8;

    //	PIProgressInit(VS_PROGRESS_STATUS_BAR, _T("Curves"));

    float variable = 255.0f / float(_arrayNum - 1);

    if (_channel == CURVE_CHANNEL_GRAY)
    {
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width * depth; i++)
            {
                uchar* dstPixel = dstData + j * pitch + i;
                uchar* imagePixel = imageData + j * pitch + i;
                *(dstPixel) = interpolation(*imagePixel, _arrayIntensity, _arrayNum, variable);
            }

            //	PIProgressSetPercent((j + 1), height);
        }
    }
    else
    {
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                uchar* dstPixel = dstData + j * pitch + i * depth;
                uchar* imagePixel = imageData + j * pitch + i * depth;
                *(dstPixel) = interpolation(*(imagePixel), _arrayBlue, _arrayNum, variable);
                *(dstPixel + 1) = interpolation(*(imagePixel + 1), _arrayGreen, _arrayNum, variable);
                *(dstPixel + 2) = interpolation(*(imagePixel + 2), _arrayRed, _arrayNum, variable);
            }

            //	PIProgressSetPercent((j + 1), height);
        }
    }
}

void CurvesProcessor::processImageImpl(MonoImage* image, QImage* dstImage)
{
    assert(image);

    int width, height;
    uchar* byteImage = image->getBYTEImage(width, height);
    float maxValue = image->getMaxValue();
    float minValue = image->getMinValue();

    float fVariable1 = float(maxValue - minValue) / (_arrayNum - 1);
    float fVariable2 = 255.0f / (_arrayNum - 1);

    if (_channel == CURVE_CHANNEL_GRAY)
    {
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                int n = j * width + i;
                byteImage[3 * n] = byteImage[3 * n + 1] = byteImage[3 * n + 2] =
                    interpolation(image->getValue(n) - minValue, _arrayIntensity, _arrayNum, fVariable1, fVariable2);
            }

            //	PIProgressSetPercent(j + 1, height);
        }
    }
    else
    {
        // Pseudo-color
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                int n = j * width + i;
                byteImage[3 * n] = interpolation(image->getValue(n) - minValue, _arrayRed, _arrayNum, fVariable1, fVariable2);
                byteImage[3 * n + 1] = interpolation(image->getValue(n) - minValue, _arrayGreen, _arrayNum, fVariable1, fVariable2);
                byteImage[3 * n + 2] = interpolation(image->getValue(n) - minValue, _arrayBlue, _arrayNum, fVariable1, fVariable2);
            }

            //	PIProgressSetPercent(j + 1, height);
        }
    }

    image->copyByteToImage(dstImage);

    //	PIProgressDone();
}

// Process float array
void CurvesProcessor::processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(minValue);
    Q_UNUSED(maxValue);
    assert(array && pByte);
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
