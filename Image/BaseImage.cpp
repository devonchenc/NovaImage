#include "BaseImage.h"

#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmjpeg/djdecode.h>
#include <string>
#include <QDebug>
#include <QFile>

BaseImage::BaseImage()
    : _pImage(nullptr)
    , _width(0)
    , _height(0)
    , _windowWidth(0)
    , _windowLevel(0)
    , _openSucceed(false)
{
    memset(_grayPixelNumber, 0, sizeof(uint) * 256);
}

BaseImage::BaseImage(const QString& pathName)
    : _pImage(nullptr)
    , _width(0)
    , _height(0)
    , _windowWidth(0)
    , _windowLevel(0)
    , _pathName(pathName)
    , _openSucceed(false)
{
}

BaseImage::BaseImage(const BaseImage& src)
    : _width(src._width)
    , _height(src._height)
    , _windowWidth(src._windowWidth)
    , _windowLevel(src._windowLevel)
    , _pathName(src._pathName)
    , _openSucceed(src._openSucceed)
{
    _pImage.reset(new QImage(*src._pImage));

    memcpy(_grayPixelNumber, src._grayPixelNumber, sizeof(uint) * 256);
    memcpy(_redPixelNumber, src._redPixelNumber, sizeof(uint) * 256);
    memcpy(_greenPixelNumber, src._greenPixelNumber, sizeof(uint) * 256);
    memcpy(_bluePixelNumber, src._bluePixelNumber, sizeof(uint) * 256);
}

BaseImage& BaseImage::operator=(const BaseImage& src)
{
    if (this == &src)
        return *this;

    _pImage.reset(new QImage(*src._pImage));
    _width = src._width;
    _height = src._height;
    _windowWidth = src._windowWidth;
    _windowLevel = src._windowLevel;

    memcpy(_grayPixelNumber, src._grayPixelNumber, sizeof(uint) * 256);
    memcpy(_redPixelNumber, src._redPixelNumber, sizeof(uint) * 256);
    memcpy(_greenPixelNumber, src._greenPixelNumber, sizeof(uint) * 256);
    memcpy(_bluePixelNumber, src._bluePixelNumber, sizeof(uint) * 256);

    return *this;
}

float BaseImage::getValue(float x, float y) const
{
    int x0 = (int)floor(x);
    int x1 = (int)ceil(x);
    int y0 = (int)floor(y);
    int y1 = (int)ceil(y);

    float lambda_x = x - x0;
    float lambda_y = y - y0;

    float v00 = getValue(x0, y0);
    float v10 = getValue(x1, y0);
    float v01 = getValue(x0, y1);
    float v11 = getValue(x1, y1);

    // Bilinear interpolation
    // f(x,y)=f(0,0)(1-x)(1-y)+f(0,1)(1-x)y+f(1,1)xy+f(1,0)x(1-y)
    float value = (1 - lambda_x) * (1 - lambda_y) * v00 + lambda_x * (1 - lambda_y) * v10
            + (1 - lambda_x) * lambda_y * v01 + lambda_x * lambda_y * v11;
    return value;
}

QRgb BaseImage::getPixel(const QPoint& position) const
{
    return _pImage->pixel(position);
}

QRgb BaseImage::getPixel(int x, int y) const
{
    return _pImage->pixel(QPoint(x, y));
}

bool BaseImage::save(const QString& fileName)
{
    if (fileName.endsWith("dcm", Qt::CaseInsensitive))
    {
        return saveAsDcm(fileName);
    }
    else if (fileName.endsWith("raw", Qt::CaseInsensitive))
    {
        return saveAsRaw(fileName);
    }
    else
    {
        return getImageEntity()->save(fileName);
    }
}

void BaseImage::copyToArray(uchar* array) const
{
    const uchar* pData = _pImage->bits();
    int pitch = _pImage->bytesPerLine();
    int depth = _pImage->depth() / 8;
    for (int j = 0; j < _height; j++)
    {
        for (int i = 0; i < _width; i++)
        {
            const uchar* pixel = pData + j * pitch + i * depth;
            array[j * _width + i] = *pixel;
        }
    }
}

bool BaseImage::copyFromArray(const uchar* array, int width, int height)
{
    if (array == nullptr || _pImage == nullptr)
        return false;

    if (width != this->width() || height != this->height())
        return false;

    uchar* pData = _pImage->bits();
    int pitch = _pImage->bytesPerLine();
    int depth = _pImage->depth() / 8;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            uchar* pixel = pData + j * pitch + i * depth;
            for (int n = 0; n < qMin(depth, 3); n++)
            {
                *(pixel + n) = array[j * width + i];
            }
        }
    }

    return true;
}

bool BaseImage::saveAsDcm(const QString& fileName)
{
    Uint8* buffer = new Uint8[_width * _height];
    for (int j = 0; j < _height; j++)
    {
        for (int i = 0; i < _width; i++)
        {
            QRgb value = getPixel(QPoint(i, j));
            buffer[j * _width + i] = qRed(value);
        }
    }

    DcmFileFormat fileformat;
    DcmDataset* dataset = fileformat.getDataset();
    OFCondition condition = dataset->putAndInsertUint8Array(DCM_PixelData, buffer, _width * _height, true);
    delete[] buffer;
    if (condition.bad())
    {
        qDebug() << "Error: cannot insert array (" << condition.text() << ")";
        return false;
    }

    condition = dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
    if (condition.bad())
    {
        qDebug() << "Error: cannot choose representation (" << condition.text() << ")";
        return false;
    }

    dataset->putAndInsertString(DCM_Modality, "CT");
    dataset->putAndInsertUint16(DCM_Columns, _width);
    dataset->putAndInsertUint16(DCM_Rows, _height);
    dataset->putAndInsertUint16(DCM_NumberOfFrames, 1);
    dataset->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
    dataset->putAndInsertUint16(DCM_SamplesPerPixel, 1);
    dataset->putAndInsertUint16(DCM_BitsAllocated, 8);
    dataset->putAndInsertUint16(DCM_BitsStored, 8);
    dataset->putAndInsertUint16(DCM_HighBit, 7);
    dataset->putAndInsertString(DCM_PixelRepresentation, "0");
    condition = dataset->saveFile(fileName.toStdString().c_str(), EXS_LittleEndianExplicit);
    return condition.good();
}

bool BaseImage::saveAsRaw(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
        return false;

    uchar* buffer = new uchar[_width * _height];
    for (int j = 0; j < _height; j++)
    {
        for (int i = 0; i < _width; i++)
        {
            QRgb value = getPixel(QPoint(i, j));
            buffer[j * _width + i] = qRed(value);
        }
    }

    file.write((const char*)buffer, sizeof(uchar) * _width * _height);
    file.close();

    delete[] buffer;

    return true;
}
