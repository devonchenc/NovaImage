﻿#pragma once

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

    ImageDataTemplate& operator=(const ImageDataTemplate& src);

public:
    // Get original data pointer
    void* getOriginalData() override
    {
        return static_cast<void*>(_originalData.get());
    }

    // Get processing data pointer
    void* getProcessingData() override
    {
        return static_cast<void*>(_axialData);
    }

    // Get pixel value of processing data
    float getProcessingValue(int type, int index) override;

    // Set pixel value of processing data
    void setProcessingValue(int type, int index, float value) override;

    // Find top and bottom value in data
    // bool findTopAndBottom(Type* pData, int num);
    bool findTopAndBottom() override;

    // Allocate memory
    bool allocateMemory() override;

    // Convert float data to uchar data
    bool convertToByte(float* data, int size, uchar* byteImage) override;

    bool convertToByte(int type, uchar* byteImage) override;

    // Save array to QFile
    void saveArray(QFile& file) override;

    // Rescale array
    void rescaleArray(float rescaleSlope, float rescaleIntercept) override;
    void rescaleArray(int type, float rescaleSlope, float rescaleIntercept) override;

    // Rescale top and bottom
    void rescaleTopAndBottom(float rescaleSlope, float rescaleIntercept) override;

    // Create a deep copy of image data
    ImageData* copyImageData() const override;

    void restoreData() override;

    void changeSlice(int type, int slice) override;

    int getElementSize() override { return sizeof(Type); }

    void interpolateData(int index, void* prevData, void* nextData, float ratio) override;

protected:
    std::shared_ptr<Type> _originalData;

    float* _axialData;
    float* _coronalData;
    float* _sagittalData;

    int _currentAxialSlice;
    int _currentCoronalSlice;
    int _currentSagittalSlice;
};

template <class Type>
ImageDataTemplate<Type>::ImageDataTemplate(int width, int height, int slice)
    : ImageData(width, height, slice)
    , _originalData(std::shared_ptr<Type>(new Type[_totalPixel]))
    , _axialData(nullptr)
    , _coronalData(nullptr)
    , _sagittalData(nullptr)
    , _currentAxialSlice(round(slice / 2.0) - 1)
    , _currentCoronalSlice(round(height / 2.0) - 1)
    , _currentSagittalSlice(round(width / 2.0) - 1)
{
    memset(_originalData.get(), 0, sizeof(Type) * _totalPixel);
}

template <class Type>
ImageDataTemplate<Type>::ImageDataTemplate(const ImageDataTemplate& src)
    : ImageData(src)
    , _originalData(src._originalData)
    , _axialData(nullptr)
    , _coronalData(nullptr)
    , _sagittalData(nullptr)
    , _currentAxialSlice(src._currentAxialSlice)
    , _currentCoronalSlice(src._currentCoronalSlice)
    , _currentSagittalSlice(src._currentSagittalSlice)
{
    if (src._axialData)
    {
        _axialData = new float[_pixelPerSlice];
        memcpy(_axialData, src._axialData, sizeof(float) * _pixelPerSlice);
    }
    if (src._coronalData)
    {
        _coronalData = new float[_width * _slice];
        memcpy(_coronalData, src._coronalData, sizeof(float) * _width * _slice);
    }
    if (src._sagittalData)
    {
        _sagittalData = new float[_height * _slice];
        memcpy(_sagittalData, src._sagittalData, sizeof(float) * _height * _slice);
    }
}

template <class Type>
ImageDataTemplate<Type>::~ImageDataTemplate()
{
    if (_axialData)
    {
        delete[] _axialData;
        _axialData = nullptr;
    }
    if (_coronalData)
    {
        delete[] _coronalData;
        _coronalData = nullptr;
    }
    if (_sagittalData)
    {
        delete[] _sagittalData;
        _sagittalData = nullptr;
    }
}

template <class Type>
ImageDataTemplate<Type>& ImageDataTemplate<Type>::operator=(const ImageDataTemplate<Type>& src)
{
    if (this == &src)
        return *this;

    ImageData::operator=(src);

    _originalData = src._originalData;

    memcpy(_axialData, src._axialData, sizeof(float) * _pixelPerSlice);
    memcpy(_coronalData, src._coronalData, sizeof(float) * _width * _slice);
    memcpy(_sagittalData, src._sagittalData, sizeof(float) * _height * _slice);

    _currentAxialSlice = src._currentAxialSlice;
    _currentCoronalSlice = src._currentCoronalSlice;
    _currentSagittalSlice = src._currentSagittalSlice;

    return *this;
}

template <class Type>
float ImageDataTemplate<Type>::getProcessingValue(int type, int index)
{
    if (type == 0)
    {
        assert((unsigned long)index < _pixelPerSlice);
        return _axialData[index];
    }
    else if (type == 1)
    {
        assert(index < _width * _slice);
        return _coronalData[index];
    }
    else/* if (type == 2)*/
    {
        assert(index < _height * _slice);
        return _sagittalData[index];
    }
}

// Set pixel value of processing data
template <class Type>
void ImageDataTemplate<Type>::setProcessingValue(int type, int index, float value)
{
    if (type == 0)
    {
        assert((unsigned long)index < _pixelPerSlice);
        _axialData[index] = value;
    }
    else if (type == 1)
    {
        assert(index < _width * _slice);
        _coronalData[index] = value;
    }
    else/* if (type == 2)*/
    {
        assert(index < _height * _slice);
        _sagittalData[index] = value;
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
        _axialData = new float[_pixelPerSlice];
        for (unsigned long i = 0; i < _pixelPerSlice; i++)
        {
            _axialData[i] = _originalData.get()[i + _currentAxialSlice * _pixelPerSlice];
        }

        _coronalData = new float[_width * _slice];
        for (int j = 0; j < _slice; j++)
        {
            qint64 offset = j * _pixelPerSlice;
            for (int i = 0; i < _width; i++)
            {
                _coronalData[j * _width + i] = _originalData.get()[i + _currentCoronalSlice * _width + offset];
            }
        }

        _sagittalData = new float[_height * _slice];
        for (int j = 0; j < _slice; j++)
        {
            qint64 offset = j * _pixelPerSlice;
            for (int i = 0; i < _height; i++)
            {
                _sagittalData[j * _height + i] = _originalData.get()[_width * i + _currentSagittalSlice + offset];
            }
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
bool ImageDataTemplate<Type>::convertToByte(float* data, int size, uchar* byteImage)
{
    if (data == nullptr)
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

    for (int i = 0; i < size; i++)
    {
        byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] =
                uchar((data[i] - _minValue) * variable);
    }

    return true;
}

template <class Type>
bool ImageDataTemplate<Type>::convertToByte(int type, uchar* byteImage)
{
    if (type == 0)
    {
        return convertToByte(_axialData, _pixelPerSlice, byteImage);
    }
    else if (type == 1)
    {
        return convertToByte(_coronalData, _width * _slice, byteImage);
    }
    else/* if (type == 2)*/
    {
        return convertToByte(_sagittalData, _height * _slice, byteImage);
    }
}

// Save array to QFile
template <class Type>
void ImageDataTemplate<Type>::saveArray(QFile& file)
{
 //   file.write((const char*)_axialData, sizeof(float) * _pixelPerSlice);
    file.write((const char*)_originalData.get(), sizeof(Type) * _totalPixel);
}

// Rescale array
template <class Type>
void ImageDataTemplate<Type>::rescaleArray(float rescaleSlope, float rescaleIntercept)
{
    if (_axialData)
    {
        for (unsigned long i = 0; i < _pixelPerSlice; i++)
        {
            _axialData[i] = _originalData.get()[i + _currentAxialSlice * _pixelPerSlice] * rescaleSlope + rescaleIntercept;
        }
    }

    if (_coronalData)
    {
        for (int j = 0; j < _slice; j++)
        {
            qint64 offset = j * _pixelPerSlice;
            for (int i = 0; i < _width; i++)
            {
                _coronalData[j * _width + i] = _originalData.get()[i + _currentCoronalSlice * _width + offset] * rescaleSlope + rescaleIntercept;
            }
        }
    }

    if (_sagittalData)
    {
        for (int j = 0; j < _slice; j++)
        {
            qint64 offset = j * _pixelPerSlice;
            for (int i = 0; i < _height; i++)
            {
                _sagittalData[j * _height + i] = _originalData.get()[_width * i + _currentSagittalSlice + offset] * rescaleSlope + rescaleIntercept;
            }
        }
    }
}

template <class Type>
void ImageDataTemplate<Type>::rescaleArray(int type, float rescaleSlope, float rescaleIntercept)
{
    if (type == 0)
    {
        for (unsigned long i = 0; i < _pixelPerSlice; i++)
        {
            _axialData[i] = _originalData.get()[i + _currentAxialSlice * _pixelPerSlice] * rescaleSlope + rescaleIntercept;
        }
    }
    else if (type == 1)
    {
        for (int j = 0; j < _slice; j++)
        {
            qint64 offset = j * _pixelPerSlice;
            for (int i = 0; i < _width; i++)
            {
                _coronalData[j * _width + i] = _originalData.get()[i + _currentCoronalSlice * _width + offset] * rescaleSlope + rescaleIntercept;
            }
        }
    }
    else if (type == 2)
    {
        for (int j = 0; j < _slice; j++)
        {
            qint64 offset = j * _pixelPerSlice;
            for (int i = 0; i < _height; i++)
            {
                _sagittalData[j * _height + i] = _originalData.get()[_width * i + _currentSagittalSlice + offset] * rescaleSlope + rescaleIntercept;
            }
        }
    }
}

// Rescale top and bottom
template <class Type>
void ImageDataTemplate<Type>::rescaleTopAndBottom(float rescaleSlope, float rescaleIntercept)
{
    _minValue = _minValue * rescaleSlope + rescaleIntercept;
    _maxValue = _maxValue * rescaleSlope + rescaleIntercept;
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
        _axialData[i] = _originalData.get()[i + _currentAxialSlice * _pixelPerSlice];
    }
    for (int j = 0; j < _slice; j++)
    {
        qint64 offset = j * _pixelPerSlice;
        for (int i = 0; i < _width; i++)
        {
            _coronalData[j * _width + i] = _originalData.get()[i + _currentCoronalSlice * _width + offset];
        }
    }
    for (int j = 0; j < _slice; j++)
    {
        qint64 offset = j * _pixelPerSlice;
        for (int i = 0; i < _height; i++)
        {
            _sagittalData[j * _height + i] = _originalData.get()[_width * i + _currentSagittalSlice + offset];
        }
    }
}

template <class Type>
void ImageDataTemplate<Type>::changeSlice(int type, int slice)
{
    if (type == 0)
    {
        _currentAxialSlice = slice;
        for (unsigned long i = 0; i < _pixelPerSlice; i++)
        {
            _axialData[i] = _originalData.get()[i + _currentAxialSlice * _pixelPerSlice];
        }
    }
    else if (type == 1)
    {
        _currentCoronalSlice = slice;
        for (int j = 0; j < _slice; j++)
        {
            qint64 offset = j * _pixelPerSlice;
            for (int i = 0; i < _width; i++)
            {
                _coronalData[j * _width + i] = _originalData.get()[i + _currentCoronalSlice * _width + offset];
            }
        }
    }
    else/* if (type == 2)*/
    {
        _currentSagittalSlice = slice;
        for (int j = 0; j < _slice; j++)
        {
            qint64 offset = j * _pixelPerSlice;
            for (int i = 0; i < _height; i++)
            {
                _sagittalData[j * _height + i] = _originalData.get()[_width * i + _currentSagittalSlice + offset];
            }
        }
    }
}

template <class Type>
void ImageDataTemplate<Type>::interpolateData(int index, void* prevData, void* nextData, float ratio)
{
    assert(prevData);
    assert(nextData);
    qint64 offset = index * _width * _height;

    for (unsigned long i = 0; i < _pixelPerSlice; i++)
    {
        _originalData.get()[i + offset] = Type(round(((Type*)prevData)[i] * (1.0f - ratio) + ((Type*)nextData)[i] * ratio));
    }
}
