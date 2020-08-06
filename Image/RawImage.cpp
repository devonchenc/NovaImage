#include "RawImage.h"

#include <QFile>

#include "ImageDataTemplate.h"
#include "ImageReader.h"
#include "../Widget/ProgressDialog.h"

RawImage::RawImage(const QString& pathName, int type, int width, int height, int slice, int headerSize)
    : MonoImage(pathName)
    , _dataType(type)
    , _headerSize(headerSize)
{
    _width = width;
    _height = height;
    _slice = slice;
    _currentSlice = round(_slice / 2.0) - 1;

    // Read data
    if (readData() == false)
    {
        _openSucceed = false;
        return;
    }
    // Allocate memory
    if (allocateMemory() == false)
    {
        _openSucceed = false;
        return;
    }

    initWindowWidthAndLevel();

    // Convert float data to uchar data
    if (convertToByte() == false)
    {
        _openSucceed = false;
        return;
    }
    // Copy to image
    if (copyToImage() == false)
    {
        _openSucceed = false;
        return;
    }

    _openSucceed = true;
}

RawImage::RawImage(const RawImage& src)
    : MonoImage(src)
    , _dataType(src._dataType)
    , _headerSize(src._headerSize)
{

}

RawImage::~RawImage()
{

}

void RawImage::setSlice(int slice)
{
    MonoImage::setSlice(slice);

    convertToByte();

    copyToImage();
}

void RawImage::initWindowWidthAndLevel()
{
    _windowWidth = _imageData->getMaximumValue() - _imageData->getMinimumValue();
    _windowLevel = (_imageData->getMaximumValue() + _imageData->getMinimumValue()) / 2.0f;
}

BaseImage* RawImage::copyImage() const
{
    return new RawImage(*this);
}

// Read data
bool RawImage::readData()
{
    if (_width <= 0 || _height <= 0 || _slice <= 0)
        return false;

    AbstractReader* reader = nullptr;
    switch (_dataType)
    {
    case 0:
    {
        _imageData = new ImageDataTemplate<uchar>(_width * _height, _slice);
        uchar* originalData = static_cast<uchar*>(_imageData->getOriginalData());
        reader = new ImageReader<uchar>(_pathName, _headerSize, _width * _height, _slice, originalData);
    }
    break;
    case 1:
    {
        _imageData = new ImageDataTemplate<ushort>(_width * _height, _slice);
        ushort* originalData = static_cast<ushort*>(_imageData->getOriginalData());
        reader = new ImageReader<ushort>(_pathName, _headerSize, _width * _height, _slice, originalData);
    }
    break;
    case 2:
    {
        _imageData = new ImageDataTemplate<uint>(_width * _height, _slice);
        uint* originalData = static_cast<uint*>(_imageData->getOriginalData());
        reader = new ImageReader<uint>(_pathName, _headerSize, _width * _height, _slice, originalData);
    }
    break;
    case 3:
    {
        _imageData = new ImageDataTemplate<float>(_width * _height, _slice);
        float* originalData = static_cast<float*>(_imageData->getOriginalData());
        reader = new ImageReader<float>(_pathName, _headerSize, _width * _height, _slice, originalData);
    }
    break;
    case 4:
    {
        _imageData = new ImageDataTemplate<double>(_width * _height, _slice);
        double* originalData = static_cast<double*>(_imageData->getOriginalData());
        reader = new ImageReader<double>(_pathName, _headerSize, _width * _height, _slice, originalData);
    }
    break;
    }
    ProgressDialog dlg(reader);
    reader->setWidget(&dlg);
    reader->start();
    reader->deleteLater();

    if (dlg.exec() == QDialog::Accepted)
    {
        // Get min and max value in data
        _imageData->setMinimumValue(reader->minValue());
        _imageData->setMaximumValue(reader->maxValue());

        return true;
    }
    else
    {
        return false;
    }
}
