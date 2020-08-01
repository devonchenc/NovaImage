#pragma once

QT_BEGIN_NAMESPACE
class QFile;
QT_END_NAMESPACE

typedef unsigned char uchar;

class ImageData
{
public:
    ImageData(unsigned long pixelPerSlice, int slice = 1)
        : _pixelPerSlice(pixelPerSlice)
        , _slice(slice)
    {

    }

    ImageData(const ImageData& src)
        : _pixelPerSlice(src._pixelPerSlice)
        , _slice(src._slice)
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
    virtual float getProcessingValue(int index) = 0;

    // Allocate memory
    virtual bool allocateMemory() = 0;

    // Find top and bottom value in data
    virtual bool findTopAndBottom() = 0;

    // Allocate data to byte
    virtual bool convertToByte(uchar* byteImage) = 0;

    // Save array to QFile
    virtual void saveArray(QFile& file) = 0;

    // Rescale array
    virtual void rescaleArray(float rescaleSlope, float rescaleIntercept) = 0;

    // Create a deep copy of image data
    virtual ImageData* copyImageData() const = 0;

    virtual void restoreData() = 0;

    virtual void changeSlice(int slice) = 0;

public:
    float getMinimumValue() const { return _minValue; }

    void setMinimumValue(float minValue) { _minValue = minValue; }

    float getMaximumValue()	const { return _maxValue; }

    void setMaximumValue(float maxValue) { _maxValue = maxValue; }

protected:
    unsigned long _pixelPerSlice;

    int _slice;

    float _minValue, _maxValue;
};
