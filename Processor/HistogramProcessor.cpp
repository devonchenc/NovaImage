#include "HistogramProcessor.h"

#include <cmath>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"

HistogramProcessor::HistogramProcessor(QObject* parent)
    : BaseProcessor(false, parent)
    , _array(nullptr)
{

}

HistogramProcessor::~HistogramProcessor()
{

}

void HistogramProcessor::processImage(GeneralImage* srcImage, GeneralImage* dstImage)
{
    assert(srcImage);

    int width = srcImage->width();
    int height = srcImage->height();
    QImage* imageEntity = srcImage->getImageEntity();
    uchar* srcData = imageEntity->bits();
    uchar* dstData = dstImage->getImageEntity()->bits();
    int pitch = imageEntity->bytesPerLine();
    int depth = imageEntity->depth() / 8;

    // Statistical non-zero number
    int count = 0;
    int min = _arrayNum - 1;
    int max = 0;
    for (int i = 0; i < _arrayNum; i++)
    {
        if (_array[i] == true)
        {
            if (min > i)
            {
                min = i;
            }
            if (max < i)
            {
                max = i;
            }
            count++;
        }
    }
    if (count == 0)
    {
        // Restore image by using backup image
        *dstImage = *srcImage;
        return;
    }

    uchar actualMin = round(min * 255.0f / (_arrayNum - 1));
    uchar actualMax = round(max * 255.0f / (_arrayNum - 1));

    float variable = float(_arrayNum - 1) / 255.0f;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width * depth; i++)
        {
            uchar* dstPixel = dstData + j * pitch + i;
            uchar* imagePixel = srcData + j * pitch + i;

            if (*(imagePixel) >= actualMax)
            {
                *(dstPixel) = 255;
            }
            else if (*(imagePixel) <= actualMin)
            {
                *(dstPixel) = 0;
            }
            else
            {
                int index = round(*(imagePixel) * variable);
                if (_array[index])
                {
                    *(dstPixel) = (*(imagePixel) - actualMin) * 255 / (actualMax - actualMin);
                }
                else
                {
                    *(dstPixel) = 0;
                }
            }
        }
    }
}

void HistogramProcessor::processImage(MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
    uchar* byteImage = srcImage->getBYTEImage(width, height);
    float maxValue = srcImage->getMaxValue();
    float minValue = srcImage->getMinValue();

    // Statistical non-zero number
    int count = 0;
    int min = _arrayNum - 1;
    int max = 0;
    for (int i = 0; i < _arrayNum; i++)
    {
        if (_array[i] == true)
        {
            if (min > i)
            {
                min = i;
            }
            if (max < i)
            {
                max = i;
            }
            count++;
        }
    }
    if (count == 0)
    {
        // Convert data to byte
        srcImage->convertToByte();
        srcImage->copyByteToImage();
        return;
    }

    float actualMin = float(min * float(maxValue - minValue) / (_arrayNum - 1) + minValue);
    float actualMax = float(max * float(maxValue - minValue) / (_arrayNum - 1) + minValue);

    for (int i = 0; i < width * height; i++)
    {
        if (srcImage->getValue(i) >= actualMax)
        {
            byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 255;
        }
        else if (srcImage->getValue(i) <= actualMin)
        {
            byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 0;
        }
        else
        {
            int index = round(float(srcImage->getValue(i) - minValue) * (_arrayNum - 1) / float(maxValue - minValue));
            if (_array[index])
            {
                byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] =
                        round((srcImage->getValue(i) - actualMin) * 255.0f / (actualMax - actualMin));
            }
            else
            {
                byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 0;
            }
        }
    }

//    srcImage->copyByteToImage(dstImage);
}

// Process float array
void HistogramProcessor::processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(minValue);
    Q_UNUSED(maxValue);
    assert(array && pByte);

}

void HistogramProcessor::setWindowArray(bool* array, int arrayNum)
{
    _array = array;
    _arrayNum = arrayNum;
}
