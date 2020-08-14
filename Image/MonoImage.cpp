#include "MonoImage.h"

#include <QFile>

#include "../Core/GlobalFunc.h"
#include "ImageData.h"

MonoImage::MonoImage()
    : BaseImage()
    , _imageData(nullptr)
    , _byteTopImage(nullptr)
    , _byteFrontalImage(nullptr)
    , _byteProfileImage(nullptr)
    , _slice(1)
    , _currentSlice(0)
{

}

MonoImage::MonoImage(const QString& pathName)
    : BaseImage(pathName)
    , _imageData(nullptr)
    , _byteTopImage(nullptr)
    , _byteFrontalImage(nullptr)
    , _byteProfileImage(nullptr)
    , _slice(1)
    , _currentSlice(0)
{

}

MonoImage::MonoImage(const MonoImage& src)
    : BaseImage(src)
    , _slice(src._slice)
    , _currentSlice(src._currentSlice)
{
    _imageData = src._imageData->copyImageData();

    _byteTopImage = new uchar[_width * _height * 3];
    memcpy(_byteTopImage, src._byteTopImage, sizeof(uchar) * _width * _height * 3);

    if (src._byteFrontalImage)
    {
        _byteFrontalImage = new uchar[_width * _slice * 3];
        memcpy(_byteFrontalImage, src._byteFrontalImage, sizeof(uchar) * _width * _slice * 3);
    }

    if (src._byteProfileImage)
    {
        _byteProfileImage = new uchar[_height * _slice * 3];
        memcpy(_byteProfileImage, src._byteProfileImage, sizeof(uchar) * _height * _slice * 3);
    }
}

MonoImage::~MonoImage()
{
    if (_imageData)
    {
        delete _imageData;
        _imageData = nullptr;
    }

    if (_byteTopImage)
    {
        delete[] _byteTopImage;
        _byteTopImage = nullptr;
    }
    if (_byteFrontalImage)
    {
        delete[] _byteFrontalImage;
        _byteFrontalImage = nullptr;
    }
    if (_byteProfileImage)
    {
        delete[] _byteProfileImage;
        _byteProfileImage = nullptr;
    }
}

bool MonoImage::copyToImage()
{
    return copyByteToImage(_byteTopImage, _width, _height, _pImage);
}

void MonoImage::setSlice(int slice)
{
    _currentSlice = slice;

    _imageData->changeSlice(_currentSlice);
}

void MonoImage::histogramStatistic()
{
    memset(_grayPixelNumber, 0, sizeof(uint) * 256);
    memset(_redPixelNumber, 0, sizeof(uint) * 256);
    memset(_greenPixelNumber, 0, sizeof(uint) * 256);
    memset(_bluePixelNumber, 0, sizeof(uint) * 256);

    float minValue = _imageData->getMinimumValue();
    float maxValue = _imageData->getMaximumValue();
    if (minValue != maxValue)
    {
        float temp = 255.0f / (maxValue - minValue);
        for (int j = 0; j < _height; j++)
        {
            for (int i = 0; i < _width; i++)
            {
                float value = getValue(QPoint(i, j));
                int index = int((value - minValue) * temp);
                _grayPixelNumber[index]++;
            }
        }
        // Clear the count numbers of pixels whose gray value less than 4
        memset(_grayPixelNumber, 0, sizeof(uint) * 3);

        memcpy(_redPixelNumber, _grayPixelNumber, sizeof(uint) * 256);
        memcpy(_greenPixelNumber, _grayPixelNumber, sizeof(uint) * 256);
        memcpy(_bluePixelNumber, _grayPixelNumber, sizeof(uint) * 256);
    }
}

float MonoImage::getValue(const QPoint& position) const
{
    if (position.x() < 0 || position.x() >= _width || position.y() < 0 || position.y() >= _height)
        return 0;

    int index = position.y() * _width + position.x();
    return _imageData->getProcessingValue(index);
}

float MonoImage::getValue(int x, int y) const
{
    return getValue(QPoint(x, y));
}

float MonoImage::getValue(int index) const
{
    return _imageData->getProcessingValue(index);
}

float MonoImage::getMinValue() const
{
    return _imageData->getMinimumValue();
}

float MonoImage::getMaxValue() const
{
    return _imageData->getMaximumValue();
}

void MonoImage::restore()
{
    _imageData->restoreData();

    convertToByte();

    copyToImage();
}

bool MonoImage::convertToByte()
{
    return _imageData->convertToByte(_byteTopImage);
}

QImage MonoImage::getFrontalSlice()
{
//    _imageData->getFrontalSlice();
//    _imageData->convertToByte(_byteImage);

    return QImage();
}

QImage MonoImage::getProfileSlice()
{
    return QImage();
}

bool MonoImage::saveAsRaw(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
        return false;

    _imageData->saveArray(file);

    file.close();

    return true;
}

bool MonoImage::allocateMemory()
{
    _imageData->allocateMemory();

    _byteTopImage = new uchar[_width * _height * 3];

    _pImage = new QImage(_width, _height, QImage::Format_RGB888);

    return true;
}
