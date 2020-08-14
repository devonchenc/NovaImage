#pragma once

#include "ImageData.h"

#include <cmath>
#include <iostream>
#include <QFile>
#include <QMessageBox>

template <class Type>
class ImageDataTemplate : public ImageData
{
public:
    ImageDataTemplate(int width, int height, int slice = 1);
    ImageDataTemplate(const ImageDataTemplate& src);
    virtual ~ImageDataTemplate();

public:
    // Get original data pointer
    void* getOriginalData() override
    {
        return static_cast<void*>(_originalData.get());
    }

    // Get processing data pointer
    void* getProcessingData() override
    {
        return static_cast<void*>(_processingData);
    }

    // Get pixel value of processing data
    float getProcessingValue(int index) override
    {
        return _processingData[index];
    }

    // Find top and bottom value in data
    //	bool findTopAndBottom(Type* pData, int num);
    bool findTopAndBottom() override;

    // Allocate memory
    bool allocateMemory() override;

    // Convert float data to uchar data
    bool convertToByte(uchar* byteImage) override;

    // Save array to QFile
    void saveArray(QFile& file) override;

    // Rescale array
    void rescaleArray(float rescaleSlope, float rescaleIntercept) override;

    // Create a deep copy of image data
    ImageData* copyImageData() const override;

    void restoreData() override;

    void changeSlice(int slice) override;

protected:
    std::shared_ptr<Type> _originalData;

    float* _processingData;
    float* _frontalData;
    float* _profileData;

    int _currentSlice;
};

template <class Type>
ImageDataTemplate<Type>::ImageDataTemplate(int width, int height, int slice)
    : ImageData(width, height, slice)
    , _originalData(std::shared_ptr<Type>(new Type[_totalPixel]))
    , _processingData(nullptr)
    , _frontalData(nullptr)
    , _profileData(nullptr)
    , _currentSlice(round(slice / 2.0) - 1)
{

}

template <class Type>
ImageDataTemplate<Type>::ImageDataTemplate(const ImageDataTemplate& src)
    : ImageData(src)
    , _originalData(src._originalData)
    , _currentSlice(src._currentSlice)
{
    if (src._processingData)
    {
        _processingData = new float[_pixelPerSlice];
        memcpy(_processingData, src._processingData, sizeof(float) * _pixelPerSlice);
    }
    if (src._frontalData)
    {
        _frontalData = new float[_width * _slice];
        memcpy(_frontalData, src._frontalData, sizeof(float) * _width * _slice);
    }
    if (src._profileData)
    {
        _profileData = new float[_height * _slice];
        memcpy(_profileData, src._profileData, sizeof(float) * _height * _slice);
    }
}

template <class Type>
ImageDataTemplate<Type>::~ImageDataTemplate()
{
    if (_processingData)
    {
        delete [] _processingData;
        _processingData = nullptr;
    }
}

// Traverse the max and min values in array
template <class Type>
bool ImageDataTemplate<Type>::findTopAndBottom()
{
    _minValue = _maxValue = _originalData.get()[0];
    for (unsigned long i = 1; i < _totalPixel; i++)
    {
        if ((std::is_same<Type, float>::value || std::is_same<Type, double>::value) && (std::isnan(_originalData.get()[i]) || std::isinf(_originalData.get()[i])))
        {
            QMessageBox::critical(nullptr, "Error in traversing data", "Invalid value in data!", QMessageBox::Ok);
            return false;
        }

        if (_minValue > _originalData.get()[i])
        {
            _minValue = _originalData.get()[i];
        }
        if (_maxValue < _originalData.get()[i])
        {
            _maxValue = _originalData.get()[i];
        }
    }

    return true;
}

// Allocate memory
template <class Type>
bool ImageDataTemplate<Type>::allocateMemory()
{
    try
    {
        _processingData = new float[_pixelPerSlice];
        for (unsigned long i = 0; i < _pixelPerSlice; i++)
        {
            _processingData[i] = _originalData.get()[i + _currentSlice * _pixelPerSlice];
        }
    }
    catch (const std::bad_alloc& e)
    {
        std::cout << "Allocation failed: " << e.what() << std::endl;
    }

    return true;
}

// Convert data to byte
template <class Type>
bool ImageDataTemplate<Type>::convertToByte(uchar* byteImage)
{
    if (_processingData == nullptr)
        return false;

    float variable;
    if (_minValue != _maxValue)
    {
        variable = 255.0f / (_maxValue - _minValue);
    }
    else
    {
        variable = 0.0f;
    }

    for (unsigned long i = 0; i < _pixelPerSlice; i++)
    {
        byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] =
                uchar((_processingData[i] - _minValue) * variable);
    }

    return true;
}

// Save array to QFile
template <class Type>
void ImageDataTemplate<Type>::saveArray(QFile& file)
{
    file.write((const char*)_processingData, sizeof(Type) * _pixelPerSlice);
}

// Rescale array
template <class Type>
void ImageDataTemplate<Type>::rescaleArray(float rescaleSlope, float rescaleIntercept)
{
    if (_processingData)
    {
        for (unsigned long i = 0; i < _pixelPerSlice; i++)
        {
            _processingData[i] = _originalData.get()[i + _currentSlice * _pixelPerSlice] * rescaleSlope + rescaleIntercept;
        }

        _minValue = _minValue * rescaleSlope + rescaleIntercept;
        _maxValue = _maxValue * rescaleSlope + rescaleIntercept;
    }
}

// Create a deep copy of image data
template <class Type>
ImageData* ImageDataTemplate<Type>::copyImageData() const
{
    return new ImageDataTemplate<Type>(*this);
}

template <class Type>
void ImageDataTemplate<Type>::restoreData()
{
    for (unsigned long i = 0; i < _pixelPerSlice; i++)
    {
        _processingData[i] = _originalData.get()[i + _currentSlice * _pixelPerSlice];
    }
}

template <class Type>
void ImageDataTemplate<Type>::changeSlice(int slice)
{
    _currentSlice = slice;
    for (unsigned long i = 0; i < _pixelPerSlice; i++)
    {
        _processingData[i] = _originalData.get()[i + _currentSlice * _pixelPerSlice];
    }
}