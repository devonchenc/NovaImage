#include "MonoImage.h"

#include <QFile>

#include "../Core/GlobalFunc.h"
#include "ImageData.h"
#include "MonoImageProxy.h"

MonoImage::MonoImage()
    : BaseImage()
    , _imageData(nullptr)
    , _axialProxy(nullptr)
    , _coronalProxy(nullptr)
    , _sagittalProxy(nullptr)
    , _currentViewType(AXIAL_VIEW)
    , _slice(1)
    , _currentAxialSlice(0)
    , _currentCoronalSlice(0)
    , _currentSagittalSlice(0)
{

}

MonoImage::MonoImage(const QString& pathName)
    : BaseImage(pathName)
    , _imageData(nullptr)
    , _axialProxy(nullptr)
    , _coronalProxy(nullptr)
    , _sagittalProxy(nullptr)
    , _currentViewType(AXIAL_VIEW)
    , _slice(1)
    , _currentAxialSlice(0)
    , _currentCoronalSlice(0)
    , _currentSagittalSlice(0)
{

}

MonoImage::MonoImage(const MonoImage& src)
    : BaseImage(src)
    , _axialProxy(nullptr)
    , _coronalProxy(nullptr)
    , _sagittalProxy(nullptr)
    , _currentViewType(src._currentViewType)
    , _slice(src._slice)
    , _currentAxialSlice(src._currentAxialSlice)
    , _currentCoronalSlice(src._currentCoronalSlice)
    , _currentSagittalSlice(src._currentSagittalSlice)
{
    _imageData = src._imageData->copyImageData();

    _axialProxy = new MonoImageProxy(*src._axialProxy);

    if (src._coronalProxy)
    {
        _coronalProxy = new MonoImageProxy(*src._coronalProxy);
    }

    if (src._sagittalProxy)
    {
        _sagittalProxy = new MonoImageProxy(*src._sagittalProxy);
    }

    _pImage = _axialProxy->getImageEntity();

    _currentViewType = src._currentViewType;
    _slice = src._slice;
    _currentAxialSlice = src._currentAxialSlice;
    _currentCoronalSlice = src._currentCoronalSlice;
    _currentSagittalSlice = src._currentSagittalSlice;
}

MonoImage::~MonoImage()
{
    if (_imageData)
    {
        delete _imageData;
        _imageData = nullptr;
    }
    if (_axialProxy)
    {
        delete _axialProxy;
        _axialProxy = nullptr;
    }
    if (_coronalProxy)
    {
        delete _coronalProxy;
        _coronalProxy = nullptr;
    }
    if (_sagittalProxy)
    {
        delete _sagittalProxy;
        _sagittalProxy = nullptr;
    }
}

MonoImage& MonoImage::operator=(const MonoImage& src)
{
    if (this == &src)
        return *this;

    BaseImage::operator=(src);

    _imageData = src._imageData->copyImageData();

    *_axialProxy = *src._axialProxy;
    *_coronalProxy = *src._coronalProxy;
    *_sagittalProxy = *src._sagittalProxy;

    _pImage = _axialProxy->getImageEntity();

    _currentViewType = src._currentViewType;
    _slice = src._slice;
    _currentAxialSlice = src._currentAxialSlice;
    _currentCoronalSlice = src._currentCoronalSlice;
    _currentSagittalSlice = src._currentSagittalSlice;

    return *this;
}

bool MonoImage::copyByteToAllImage()
{
    _axialProxy->copyByteToImage();
    if (_coronalProxy)
    {
        _coronalProxy->copyByteToImage();
    }
    if (_sagittalProxy)
    {
        _sagittalProxy->copyByteToImage();
    }
    return true;
}

bool MonoImage::copyByteToImage()
{
    if (_currentViewType == AXIAL_VIEW)
    {
        _axialProxy->copyByteToImage();
    }
    else if (_currentViewType == CORONAL_VIEW)
    {
        _coronalProxy->copyByteToImage();
    }
    else/* if (_currentType == SAGITTAL_VIEW)*/
    {
        _sagittalProxy->copyByteToImage();
    }
    return true;
}

bool MonoImage::copyByteToImage(QImage* dstImage)
{
    if (_currentViewType == AXIAL_VIEW)
    {
        _axialProxy->copyByteToImage(dstImage);
    }
    else if (_currentViewType == CORONAL_VIEW)
    {
        _coronalProxy->copyByteToImage(dstImage);
    }
    else/* if (_currentType == SAGITTAL_VIEW)*/
    {
        _sagittalProxy->copyByteToImage(dstImage);
    }
    return true;
}

void MonoImage::setSlice(int slice)
{
    if (_currentViewType == AXIAL_VIEW)
    {
        _currentAxialSlice = slice;
    }
    else if (_currentViewType == CORONAL_VIEW)
    {
        _currentCoronalSlice = slice;
    }
    else/* if (_currentType == SAGITTAL_VIEW)*/
    {
        _currentSagittalSlice = slice;
    }
    _imageData->changeSlice(_currentViewType, slice);
}

void MonoImage::setSlice(int type, int slice)
{
    if (type == AXIAL_VIEW)
    {
        _currentAxialSlice = slice;
    }
    else if (type == CORONAL_VIEW)
    {
        _currentCoronalSlice = slice;
    }
    else/* if (type == SAGITTAL_VIEW)*/
    {
        _currentSagittalSlice = slice;
    }
    _imageData->changeSlice(type, slice);
}

int MonoImage::currentSlice() const
{
    if (_currentViewType == AXIAL_VIEW)
    {
        return _currentAxialSlice;
    }
    else if (_currentViewType == CORONAL_VIEW)
    {
        return _currentCoronalSlice;
    }
    else/* if (_currentType == SAGITTAL_VIEW)*/
    {
        return _currentSagittalSlice;
    }
}

int MonoImage::currentSlice(int type) const
{
    if (type == AXIAL_VIEW)
    {
        return _currentAxialSlice;
    }
    else if (type == CORONAL_VIEW)
    {
        return _currentCoronalSlice;
    }
    else/* if (type == SAGITTAL_VIEW)*/
    {
        return _currentSagittalSlice;
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
    if (position.x() < 0 || position.y() < 0)
        return 0;

    int index;
    if (_currentViewType == AXIAL_VIEW)
    {
        if (position.x() >= _width  || position.y() >= _height)
            return 0;

        index = position.y() * _width + position.x();
    }
    else if (_currentViewType == CORONAL_VIEW)
    {
        if (position.x() >= _width || position.y() >= _slice)
            return 0;

        index = position.y() * _width + position.x();
    }
    else/* if (_currentType == SAGITTAL_VIEW)*/
    {
        if (position.x() >= _height || position.y() >= _slice)
            return 0;

        index = position.y() * _height + position.x();
    }

    return _imageData->getProcessingValue(_currentViewType, index);
}

float MonoImage::getValue(int x, int y) const
{
    return getValue(QPoint(x, y));
}

float MonoImage::getValue(int index) const
{
    return _imageData->getProcessingValue(_currentViewType, index);
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

void MonoImage::setViewType(int viewType)
{
    _currentViewType = viewType;
}

QImage* MonoImage::getImageEntity() const
{
    if (_currentViewType == AXIAL_VIEW)
    {
        return _pImage.get();
    }
    else if (_currentViewType == CORONAL_VIEW)
    {
        return _coronalProxy->getImageEntity().get();
    }
    else/* if (_currentType == SAGITTAL_VIEW)*/
    {
        return _sagittalProxy->getImageEntity().get();
    }
}

void MonoImage::setValue(int index, float value)
{
    _imageData->setProcessingValue(_currentViewType, index, value);
}

uchar* MonoImage::getBYTEImage(int& width, int& height)
{
    if (_currentViewType == AXIAL_VIEW)
    {
        width = _width;
        height = _height;
        return _axialProxy->getBYTEImage();
    }
    else if (_currentViewType == CORONAL_VIEW)
    {
        width = _width;
        height = _slice;
        return _coronalProxy->getBYTEImage();
    }
    else/* if (_currentType == SAGITTAL_VIEW)*/
    {
        width = _height;
        height = _slice;
        return _sagittalProxy->getBYTEImage();
    }
}

bool MonoImage::convertAllToByte()
{
    _imageData->convertToByte(0, _axialProxy->getBYTEImage());
    if (_coronalProxy)
    {
        _imageData->convertToByte(1, _coronalProxy->getBYTEImage());
    }
    if (_sagittalProxy)
    {
        _imageData->convertToByte(2, _sagittalProxy->getBYTEImage());
    }

    return true;
}

bool MonoImage::convertToByte()
{
    if (_currentViewType == AXIAL_VIEW)
    {
        _imageData->convertToByte(0, _axialProxy->getBYTEImage());
    }
    else if (_currentViewType == CORONAL_VIEW)
    {
        _imageData->convertToByte(1, _coronalProxy->getBYTEImage());
    }
    else/* if (_currentType == SAGITTAL_VIEW)*/
    {
        _imageData->convertToByte(2, _sagittalProxy->getBYTEImage());
    }

    return true;
}

std::shared_ptr<QImage> MonoImage::getCoronalImage() const
{
    return _coronalProxy->getImageEntity();
}

std::shared_ptr<QImage> MonoImage::getSagittalImage() const
{
    return _sagittalProxy->getImageEntity();
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
    _axialProxy = new MonoImageProxy(this, _width, _height, AXIAL_VIEW);
    if (_slice > 1)
    {
        _coronalProxy = new MonoImageProxy(this, _width, _slice, CORONAL_VIEW);
        _sagittalProxy = new MonoImageProxy(this, _height, _slice, SAGITTAL_VIEW);
    }

    _pImage = _axialProxy->getImageEntity();

    _imageData->allocateMemory();

    return true;
}
