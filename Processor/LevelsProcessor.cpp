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

LevelsProcessor::~LevelsProcessor()
{

}

void LevelsProcessor::processImage(GeneralImage* srcImage, GeneralImage* dstImage)
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

    int channel = 0;

    int minColor = 0;
    int maxColor = 255;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            uchar* dstPixel = dstData + j * pitch + i * depth;
            uchar* imagePixel = srcData + j * pitch + i * depth;

            for (int n = 0; n < qMin(depth, 3); n++)
            {
                dstPixel[n] = srcImage->calcNewColor(imagePixel[n], _bottom, _mid, _top, minColor, maxColor);
            }
        }
    }
}

void LevelsProcessor::processImage(MonoImage* srcImage, MonoImage* dstImage)
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
            else	// if mid is not equal to 1
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

// Process float array
void LevelsProcessor::processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(minValue);
    Q_UNUSED(maxValue);
    assert(array && pByte);

    /*	int channel = 0;

    float variable1 = 255.0f / (float)(_top - _bottom);
    int midColor = round(255 * _mid / (1 + _mid));
    float fVariable2 = (float)(midColor) / (float)((_bottom + _top) / 2.0f - _bottom);
    float fVariable3 = (float)(255.0f - midColor) / (float)(_top - (_bottom + fTop) / 2.0f);

    for (int i = 0; i < width * height; i++)
    {
        if (pArray[i] <= _bottom)
        {
            switch (nChannel)
            {
            case 0:
                pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = 0;
                break;
            case 1:
                pByte[3 * i + 2] = 0;
                break;
            case 2:
                pByte[3 * i + 1] = 0;
                break;
            case 3:
                pByte[3 * i] = 0;
                break;
            }
        }
        else if (pArray[i] >= _top)
        {
            switch (nChannel)
            {
            case 0:
                pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = 255;
                break;
            case 1:
                pByte[3 * i + 2] = 255;
                break;
            case 2:
                pByte[3 * i + 1] = 255;
                break;
            case 3:
                pByte[3 * i] = 255;
                break;
            }
        }
        else
        {
            if (_mid == 1)
            {
                uchar value = uchar((pArray[i] - fBottom) * fVariable1);
                switch (channel)
                {
                case 0:
                    pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = value;
                    break;
                case 1:
                    pByte[3 * i + 2] = value;
                    break;
                case 2:
                    pByte[3 * i + 1] = value;
                    break;
                case 3:
                    pByte[3 * i] = value;
                    break;
                }
            }
            else	// if mid is not equal to 1
            {
                if (pArray[i] < (_bottom + _top) / 2)
                {
                    uchar value = uchar((pArray[i] - fBottom) * fVariable2);
                    switch (channel)
                    {
                    case 0:
                        pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = value;
                        break;
                    case 1:
                        pByte[3 * i + 2] = value;
                        break;
                    case 2:
                        pByte[3 * i + 1] = value;
                        break;
                    case 3:
                        pByte[3 * i] = value;
                        break;
                    }
                }
                else
                {
                    uchar value = uchar((pArray[i] - (fBottom + fTop) / 2) * fVariable3 + nMidColor)
                    switch (nChannel)
                    {
                    case 0:
                        pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = value;
                        break;
                    case 1:
                        pByte[3 * i + 2] = value;
                        break;
                    case 2:
                        pByte[3 * i + 1] = value;
                        break;
                    case 3:
                        pByte[3 * i] = value;
                        break;
                    }
                }
            }
        }
    }*/
}

void LevelsProcessor::setPara(float bottom, float mid, float top)
{
    _bottom = bottom;
    _mid = mid;
    _top = top;
}
