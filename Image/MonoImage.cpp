#include "MonoImage.h"

#include <QFile>

#include "../Core/GlobalFunc.h"
#include "ImageData.h"
#include "MonoImageProxy.h"

MonoImage::MonoImage()
    : BaseImage()
    , _imageData(nullptr)
    , _topProxy(nullptr)
    , _frontalProxy(nullptr)
    , _profileProxy(nullptr)
    , _currentType(TOP_VIEW)
    , _slice(1)
    , _currentTopSlice(0)
    , _currentFrontalSlice(0)
    , _currentProfileSlice(0)
{

}

MonoImage::MonoImage(const QString& pathName)
    : BaseImage(pathName)
    , _imageData(nullptr)
    , _topProxy(nullptr)
    , _frontalProxy(nullptr)
    , _profileProxy(nullptr)
    , _currentType(TOP_VIEW)
    , _slice(1)
    , _currentTopSlice(0)
    , _currentFrontalSlice(0)
    , _currentProfileSlice(0)
{

}

MonoImage::MonoImage(const MonoImage& src)
    : BaseImage(src)
    , _topProxy(nullptr)
    , _frontalProxy(nullptr)
    , _profileProxy(nullptr)
    , _currentType(src._currentType)
    , _slice(src._slice)
    , _currentTopSlice(src._currentTopSlice)
    , _currentFrontalSlice(src._currentFrontalSlice)
    , _currentProfileSlice(src._currentProfileSlice)
{
    _imageData = src._imageData->copyImageData();

    _topProxy = new MonoImageProxy(*src._topProxy);

    if (src._frontalProxy)
    {
        _frontalProxy = new MonoImageProxy(*src._frontalProxy);
    }

    if (src._profileProxy)
    {
        _profileProxy = new MonoImageProxy(*src._profileProxy);
    }

    _pImage = _topProxy->getImageEntity();
}

MonoImage::~MonoImage()
{
    if (_imageData)
    {
        delete _imageData;
        _imageData = nullptr;
    }
    if (_topProxy)
    {
        delete _topProxy;
        _topProxy = nullptr;
    }
    if (_frontalProxy)
    {
        delete _frontalProxy;
        _frontalProxy = nullptr;
    }
    if (_profileProxy)
    {
        delete _profileProxy;
        _profileProxy = nullptr;
    }
}

bool MonoImage::copyByteToAllImage()
{
    _topProxy->copyByteToImage();
    if (_frontalProxy)
    {
        _frontalProxy->copyByteToImage();
    }
    if (_profileProxy)
    {
        _profileProxy->copyByteToImage();
    }
    return true;
}

bool MonoImage::copyByteToImage()
{
    if (_currentType == TOP_VIEW)
    {
        _topProxy->copyByteToImage();
    }
    else if (_currentType == FRONTAL_VIEW)
    {
        _frontalProxy->copyByteToImage();
    }
    else/* if (_currentType == PROFILE_VIEW)*/
    {
        _profileProxy->copyByteToImage();
    }
    return true;
}

void MonoImage::setSlice(int slice)
{
    if (_currentType == TOP_VIEW)
    {
        _currentTopSlice = slice;
        _imageData->changeSlice(TOP_VIEW, slice);
    }
    else if (_currentType == FRONTAL_VIEW)
    {
        _currentFrontalSlice = slice;
        _imageData->changeSlice(FRONTAL_VIEW, slice);
    }
    else/* if (_currentType == PROFILE_VIEW)*/
    {
        _currentProfileSlice = slice;
        _imageData->changeSlice(PROFILE_VIEW, slice);
    }
}

int MonoImage::currentSlice() const
{
    if (_currentType == TOP_VIEW)
    {
        return _currentTopSlice;
    }
    else if (_currentType == FRONTAL_VIEW)
    {
        return _currentFrontalSlice;
    }
    else/* if (_currentType == PROFILE_VIEW)*/
    {
        return _currentProfileSlice;
    }
}

int MonoImage::currentSlice(int type) const
{
    if (type == TOP_VIEW)
    {
        return _currentTopSlice;
    }
    else if (type == FRONTAL_VIEW)
    {
        return _currentFrontalSlice;
    }
    else/* if (type == PROFILE_VIEW)*/
    {
        return _currentProfileSlice;
    }
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
    return _imageData->getProcessingValue(_currentType, index);
}

float MonoImage::getValue(int x, int y) const
{
    return getValue(QPoint(x, y));
}

float MonoImage::getValue(int index) const
{
    return _imageData->getProcessingValue(_currentType, index);
}

float MonoImage::getValueWithType(int type, int index) const
{
    return _imageData->getProcessingValue(type, index);
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

    convertAllToByte();

    copyByteToAllImage();
}

void MonoImage::setViewType(int type)
{
    _currentType = type;
}

uchar* MonoImage::getBYTEImage(int& width, int& height)
{
    if (_currentType == TOP_VIEW)
    {
        width = _width;
        height = _height;
        return _topProxy->getBYTEImage();
    }
    else if (_currentType == FRONTAL_VIEW)
    {
        width = _width;
        height = _slice;
        return _frontalProxy->getBYTEImage();
    }
    else/* if (_currentType == PROFILE_VIEW)*/
    {
        width = _height;
        height = _slice;
        return _profileProxy->getBYTEImage();
    }
}

bool MonoImage::convertAllToByte()
{
    _imageData->convertToByte(0, _topProxy->getBYTEImage());
    if (_frontalProxy)
    {
        _imageData->convertToByte(1, _frontalProxy->getBYTEImage());
    }
    if (_profileProxy)
    {
        _imageData->convertToByte(2, _profileProxy->getBYTEImage());
    }

    return true;
}

bool MonoImage::convertToByte()
{
    if (_currentType == TOP_VIEW)
    {
        _imageData->convertToByte(0, _topProxy->getBYTEImage());
    }
    else if (_currentType == FRONTAL_VIEW)
    {
        _imageData->convertToByte(1, _frontalProxy->getBYTEImage());
    }
    else/* if (_currentType == PROFILE_VIEW)*/
    {
        _imageData->convertToByte(2, _profileProxy->getBYTEImage());
    }

    return true;
}

std::shared_ptr<QImage> MonoImage::getFrontalSlice() const
{
    return _frontalProxy->getImageEntity();
}

std::shared_ptr<QImage> MonoImage::getProfileSlice() const
{
    return _profileProxy->getImageEntity();
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
    _topProxy = new MonoImageProxy(this, _width, _height, TOP_VIEW);
    if (_slice > 1)
    {
        _frontalProxy = new MonoImageProxy(this, _width, _slice, FRONTAL_VIEW);
        _profileProxy = new MonoImageProxy(this, _height, _slice, PROFILE_VIEW);
    }

    _pImage = _topProxy->getImageEntity();

    _imageData->allocateMemory();

    return true;
}
