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

void HistogramProcessor::processImageImpl(GeneralImage* image, QImage* dstImage)
{
    assert(image);

    int width = image->width();
    int height = image->height();
    QImage* imageEntity = image->getImageEntity();
    uchar* imageData = imageEntity->bits();
    uchar* dstData = dstImage->bits();
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
        image->restore();
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
            uchar* imagePixel = imageData + j * pitch + i;

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

void HistogramProcessor::processImageImpl(MonoImage* image, QImage* dstImage)
{
    assert(image);

    int width, height;
    uchar* byteImage = image->getBYTEImage(width, height);
    float maxValue = image->getMaxValue();
    float minValue = image->getMinValue();

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
        image->convertToByte();
        image->copyByteToImage();
        return;
    }

    float actualMin = float(min * float(maxValue - minValue) / (_arrayNum - 1) + minValue);
    float actualMax = float(max * float(maxValue - minValue) / (_arrayNum - 1) + minValue);

    for (int i = 0; i < width * height; i++)
    {
        if (image->getValue(i) >= actualMax)
        {
            byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 255;
        }
        else if (image->getValue(i) <= actualMin)
        {
            byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 0;
        }
        else
        {
            int index = round(float(image->getValue(i) - minValue) * (_arrayNum - 1) / float(maxValue - minValue));
            if (_array[index])
            {
                byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] =
                        round((image->getValue(i) - actualMin) * 255.0f / (actualMax - actualMin));
            }
            else
            {
                byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 0;
            }
        }
    }

    image->copyByteToImage(dstImage);
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
