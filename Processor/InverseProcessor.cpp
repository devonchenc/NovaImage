#include "InverseProcessor.h"

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"

InverseProcessor::InverseProcessor(QObject* parent)
    : BaseProcessor(true, parent)
{

}

InverseProcessor::~InverseProcessor()
{

}

void InverseProcessor::processImage(GeneralImage* srcImage, GeneralImage* dstImage)
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

    if (srcData[0] == dstData[0])
    {
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
    else
    {
        *dstImage = *srcImage;
    }
}

void InverseProcessor::processImage(MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
    uchar* byteImage = srcImage->getBYTEImage(width, height);

    for (int i = 0; i < width * height * 3; i++)
    {
        byteImage[i] = 255 - byteImage[i];
    }

 //   srcImage->copyByteToImage(dstImage);
}

// Process float array
void InverseProcessor::processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(minValue);
    Q_UNUSED(maxValue);
    assert(array && pByte);

}
