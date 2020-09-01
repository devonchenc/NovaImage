#include "EqualizationProcessor.h"

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
#include "CLAHE.h"

EqualizationProcessor::EqualizationProcessor()
{

}

EqualizationProcessor::~EqualizationProcessor()
{

}

void EqualizationProcessor::processGeneralImage(GeneralImage* image)
{
    assert(image);

    int width = image->width();
    int height = image->height();
    QImage* imageEntity = image->getImageEntity();
    uchar* pImageData = (uchar*)imageEntity->bits();
    int pitch = imageEntity->bytesPerLine();
    int depth = imageEntity->depth() / 8;

//    CLAHE();
}

void EqualizationProcessor::processMonoImage(MonoImage* image)
{
    assert(image);

    int width, height;
    uchar* byteImage = image->getBYTEImage(width, height);

    uchar* temp = new uchar[width * height];
    for (int i = 0; i < width * height; i++)
    {
        temp[i] = byteImage[3 * i];
    }

    CLAHE(temp, width, height, 0, 255, 8, 8, 256, 8);

    for (int i = 0; i < width * height; i++)
    {
        byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = temp[i];
    }

    delete[] temp;

    image->copyByteToImage();
}

// Process float array
void EqualizationProcessor::processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(minValue);
    Q_UNUSED(maxValue);
    assert(array && pByte);

}
