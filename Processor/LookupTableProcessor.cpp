#include "LookupTableProcessor.h"

#include <QFile>
#include <QCoreApplication>
#include <QDebug>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"

#define HISTOGRAM_BINS      256

LookupTableProcessor::LookupTableProcessor(const QString& tableName, QObject* parent)
    : BaseProcessor(true, parent)
{
    _table = std::make_unique<unsigned char[]>(_size);
    for (int i = 0; i < _size; ++i)
    {
        _table[i] = 0;
    }

    loadLUT(tableName);
}

void LookupTableProcessor::processImage(const GeneralImage* srcImage, GeneralImage* dstImage)
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

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            uchar* pPixel = dstData + j * pitch + i * depth;
            const uchar* pBackupPixel = srcData + j * pitch + i * depth;
            for (int n = 0; n < qMin(depth, 3); n++)
            {
                *(pPixel + n) = _table[*(pBackupPixel + n) + (2 - n) * HISTOGRAM_BINS];
            }
        }
    }
}

void LookupTableProcessor::processImage(const MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
    const uchar* srcByteImage = srcImage->getBYTEImage(width, height);
    uchar* dstByteImage = dstImage->getBYTEImage(width, height);

    for (int i = 0; i < width * height; i++)
    {
        dstByteImage[3 * i] = _table[srcByteImage[3 * i] + HISTOGRAM_BINS * 2];
        dstByteImage[3 * i + 1] = _table[srcByteImage[3 * i] + HISTOGRAM_BINS];
        dstByteImage[3 * i + 2] = _table[srcByteImage[3 * i]];
    }

    dstImage->copyByteToImage();
}

bool LookupTableProcessor::loadLUT(const QString& tableName)
{
    QFile file(QCoreApplication::applicationDirPath() + "/LUT/" + tableName + ".lut");
    if (!file.open(QIODevice::ReadOnly))
        return false;

    qint64 readSize = file.read(reinterpret_cast<char*>(_table.get()), sizeof(unsigned char) * _size);
    file.close();
    if (readSize != qint64(sizeof(unsigned char)) * qint64(_size))
        return false;

    return true;
}
