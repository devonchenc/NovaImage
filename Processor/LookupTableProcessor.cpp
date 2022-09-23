#include "LookupTableProcessor.h"

#include <QFile>
#include <QCoreApplication>
#include <QDebug>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"

LookupTableProcessor::LookupTableProcessor(const QString& tableName, QObject* parent)
    : BaseProcessor(true, parent)
{
    _table = new unsigned char[_size];
    memset(_table, 0, sizeof(unsigned char) * _size);

    loadLUT(tableName);
}

LookupTableProcessor::~LookupTableProcessor()
{
    if (_table)
    {
        delete[] _table;
        _table = nullptr;
    }
}

void LookupTableProcessor::processImage(GeneralImage* srcImage, GeneralImage* dstImage)
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
                *(pPixel + n) = _table[*(pBackupPixel + n) + (2 - n) * 256];
            }
        }
    }
}

void LookupTableProcessor::processImage(MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
    uchar* srcByteImage = srcImage->getBYTEImage(width, height);
    uchar* dstByteImage = dstImage->getBYTEImage(width, height);

    for (int i = 0; i < width * height; i++)
    {
        dstByteImage[3 * i] = _table[srcByteImage[3 * i] + 512];
        dstByteImage[3 * i + 1] = _table[srcByteImage[3 * i] + 256];
        dstByteImage[3 * i + 2] = _table[srcByteImage[3 * i]];
    }

    dstImage->copyByteToImage();
}

bool LookupTableProcessor::loadLUT(const QString& tableName)
{
    QFile file(QCoreApplication::applicationDirPath() + "/LUT/" + tableName + ".lut");
    if (!file.open(QIODevice::ReadOnly))
        return false;

    qint64 readSize = file.read((char*)_table, sizeof(unsigned char) * _size);
    file.close();
    if (readSize != qint64(sizeof(unsigned char)) * qint64(_size))
        return false;

    return true;
}
