#include "LevelsProcessor.h"

#include <cmath>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"

LevelsProcessor::LevelsProcessor(QObject* parent)
    : BaseProcessor(false, parent)
    , _bottom(0)
    , _mid(0)
    , _top(0)
{
}

void LevelsProcessor::processImage(const GeneralImage* srcImage, GeneralImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width = srcImage->width();
    int height = srcImage->height();
    const QImage* imageEntity = srcImage->getImageEntity();
    const uchar* srcData = imageEntity->bits();
    uchar* dstData = dstImage->getImageEntity()->bits();
    int pitch = imageEntity->bytesPerLine();
    int depth = imageEntity->depth() / 8;

    int minColor = 0;
    int maxColor = 255;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            uchar* dstPixel = dstData + j * pitch + i * depth;
            const uchar* srcPixel = srcData + j * pitch + i * depth;

            for (int n = 0; n < qMin(depth, 3); n++)
            {
                dstPixel[n] = srcImage->calcNewColor(srcPixel[n], _bottom, _mid, _top, minColor, maxColor);
            }
        }
    }
}

void LevelsProcessor::processImage(const MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
    uchar* byteImage = dstImage->getBYTEImage(width, height);

    float variable1 = 255.0f / (float)(_top - _bottom);
    float midFactor = _mid / (1.0f + _mid);
    float variable2 = midFactor / ((_bottom + _top) / 2.0f - _bottom);
    float variable3 = (float)(1.0f - midFactor) / (float)(_top - (_bottom + _top) / 2.0f);

    for (int i = 0; i < width * height; i++)
    {
        float srcValue = srcImage->getValue(i);
        if (srcValue <= _bottom)
        {
            dstImage->setValue(i, _bottom);
            byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 0;
        }
        else if (srcValue >= _top)
        {
            dstImage->setValue(i, _top);
            byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 255;
        }
        else
        {
            if (_mid == 1)
            {
                dstImage->setValue(i, srcValue);
                uchar value = uchar((srcValue - _bottom) * variable1);
                byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = value;
            }
            else // if mid is not equal to 1
            {
                float dstValue;
                if (srcValue < (_bottom + _top) / 2.0f)
                {
                    dstValue = (srcValue - _bottom) * variable2 * (_top - _bottom) + _bottom;
                }
                else
                {
                    dstValue = ((srcValue - (_bottom + _top) / 2.0f) * variable3  + midFactor)* (_top - _bottom) + _bottom;
                    dstImage->setValue(i, dstValue);
                }
                dstImage->setValue(i, dstValue);
                uchar value = uchar((dstValue - _bottom) * variable1);
                byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = value;
            }
        }
    }

    dstImage->copyByteToImage();
}

void LevelsProcessor::setPara(float bottom, float mid, float top)
{
    _bottom = bottom;
    _mid = mid;
    _top = top;
}
