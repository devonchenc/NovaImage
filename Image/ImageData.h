#pragma once

QT_BEGIN_NAMESPACE
class QFile;
QT_END_NAMESPACE

typedef unsigned char uchar;

class ImageData
{
public:
    ImageData(int width, int height, int slice = 1)
        : _width(width)
        , _height(height)
        , _slice(slice)
        , _pixelPerSlice(width * height)
        , _totalPixel(_pixelPerSlice * slice)
    {

    }

    ImageData(const ImageData& src)
        : _width(src._width)
        , _height(src._height)
        , _slice(src._slice)
        , _pixelPerSlice(src._pixelPerSlice)
        , _totalPixel(src._totalPixel)
        , _minValue(src._minValue)
        , _maxValue(src._maxValue)
    {

    }

    virtual ~ImageData() {}

public:
    // Get original data pointer
    virtual void* getOriginalData() = 0;

    // Get processing data pointer
    virtual void* getProcessingData() = 0;

    // Get pixel value of processing data
    virtual float getProcessingValue(int type, int index) = 0;

    // Allocate memory
    virtual bool allocateMemory() = 0;

    // Find top and bottom value in data
    virtual bool findTopAndBottom() = 0;

    // Allocate data to byte
    virtual bool convertToByte(float* data, int size, uchar* byteImage) = 0;

    virtual bool convertToByte(int type, uchar* byteImage) = 0;

    // Save array to QFile
    virtual void saveArray(QFile& file) = 0;

    // Rescale array
    virtual void rescaleArray(float rescaleSlope, float rescaleIntercept) = 0;

    // Create a deep copy of image data
    virtual ImageData* copyImageData() const = 0;

    virtual void restoreData() = 0;

    virtual void changeSlice(int type, int slice) = 0;

public:
    float getMinimumValue() const { return _minValue; }

    void setMinimumValue(float minValue) { _minValue = minValue; }

    float getMaximumValue()	const { return _maxValue; }

    void setMaximumValue(float maxValue) { _maxValue = maxValue; }

protected:
    int _width, _height, _slice;

    unsigned long _pixelPerSlice;

    unsigned long _totalPixel;

    float _minValue, _maxValue;
};
