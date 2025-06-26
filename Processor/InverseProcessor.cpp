#include "InverseProcessor.h"

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"

InverseProcessor::InverseProcessor(QObject* parent)
    : BaseProcessor(true, parent)
{

}

void InverseProcessor::processImage(const GeneralImage* srcImage, GeneralImage* dstImage)
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

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            uchar* pPixel = dstData + j * pitch + i * depth;
            uchar* pBackupPixel = srcData + j * pitch + i * depth;
            for (int n = 0; n < qMin(depth, 3); n++)
            {
                *(pPixel + n) = 255 - *(pBackupPixel + n);
            }
        }
    }
}

void InverseProcessor::processImage(const MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
    const uchar* srcByteImage = srcImage->getBYTEImage(width, height);
    uchar* dstByteImage = dstImage->getBYTEImage(width, height);
    float maxValue = srcImage->getMaxValue();
    float minValue = srcImage->getMinValue();

    for (int i = 0; i < width * height; i++)
    {
        dstImage->setValue(i, maxValue + minValue - srcImage->getValue(i));
        dstByteImage[3 * i] = dstByteImage[3 * i + 1] = dstByteImage[3 * i + 2] = 255 - srcByteImage[3 * i];
    }

    dstImage->copyByteToImage();
}
