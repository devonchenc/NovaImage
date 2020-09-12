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

void InverseProcessor::processImageImpl(GeneralImage* image, QImage* dstImage)
{
    assert(image);

    int width = image->width();
    int height = image->height();
    QImage* imageEntity = image->getImageEntity();
    uchar* imageData = imageEntity->bits();
    uchar* dstData = dstImage->bits();
    int pitch = imageEntity->bytesPerLine();
    int depth = imageEntity->depth() / 8;

    if (imageData[0] == dstData[0])
    {
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                uchar* pPixel = dstData + j * pitch + i * depth;
                uchar* pBackupPixel = imageData + j * pitch + i * depth;
                for (int n = 0; n < qMin(depth, 3); n++)
                {
                    *(pPixel + n) = 255 - *(pBackupPixel + n);
                }
            }
        }
    }
    else
    {
        *dstImage = *image->getImageEntity();
    }
}

void InverseProcessor::processImageImpl(MonoImage* image, QImage* dstImage)
{
    assert(image);

    int width, height;
    uchar* byteImage = image->getBYTEImage(width, height);

    for (int i = 0; i < width * height * 3; i++)
    {
        byteImage[i] = 255 - byteImage[i];
    }

    image->copyByteToImage(dstImage);
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
