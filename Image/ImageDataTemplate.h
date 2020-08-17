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
        return static_cast<void*>(_topData);
    }

    // Get pixel value of processing data
    float getProcessingValue(int type, int index) override;

    // Find top and bottom value in data
    //	bool findTopAndBottom(Type* pData, int num);
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

    // Create a deep copy of image data
    ImageData* copyImageData() const override;

    void restoreData() override;

    void changeSlice(int type, int slice) override;

protected:
    std::shared_ptr<Type> _originalData;

    float* _topData;
    float* _frontalData;
    float* _profileData;

    int _currentTopSlice;
    int _currentFrontalSlice;
    int _currentProfileSlice;
};

template <class Type>
ImageDataTemplate<Type>::ImageDataTemplate(int width, int height, int slice)
    : ImageData(width, height, slice)
    , _originalData(std::shared_ptr<Type>(new Type[_totalPixel]))
    , _topData(nullptr)
    , _frontalData(nullptr)
    , _profileData(nullptr)
    , _currentTopSlice(round(slice / 2.0) - 1)
    , _currentFrontalSlice(round(height / 2.0) - 1)
    , _currentProfileSlice(round(width / 2.0) - 1)
{

}

template <class Type>
ImageDataTemplate<Type>::ImageDataTemplate(const ImageDataTemplate& src)
    : ImageData(src)
    , _topData(nullptr)
    , _frontalData(nullptr)
    , _profileData(nullptr)
    , _originalData(src._originalData)
    , _currentTopSlice(src._currentTopSlice)
    , _currentFrontalSlice(src._currentFrontalSlice)
    , _currentProfileSlice(src._currentProfileSlice)
{
    if (src._topData)
    {
        _topData = new float[_pixelPerSlice];
        memcpy(_topData, src._topData, sizeof(float) * _pixelPerSlice);
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
    if (_topData)
    {
        delete[] _topData;
        _topData = nullptr;
    }
    if (_frontalData)
    {
        delete[] _frontalData;
        _frontalData = nullptr;
    }
    if (_profileData)
    {
        delete[] _profileData;
        _profileData = nullptr;
    }
}

template <class Type>
float ImageDataTemplate<Type>::getProcessingValue(int type, int index)
{
    if (type == 0)
    {
        return _topData[index];
    }
    else if (type == 1)
    {
        return _frontalData[index];
    }
    else/* if (type == 2)*/
    {
        return _profileData[index];
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
        _topData = new float[_pixelPerSlice];
        for (unsigned long i = 0; i < _pixelPerSlice; i++)
        {
            _topData[i] = _originalData.get()[i + _currentTopSlice * _pixelPerSlice];
        }

        _frontalData = new float[_width * _slice];
        for (int j = 0; j < _slice; j++)
        {
            qint64 offset = j * _pixelPerSlice;
            for (int i = 0; i < _width; i++)
            {
                _frontalData[j * _width + i] = _originalData.get()[i + _currentFrontalSlice * _width + offset];
            }
        }

        _profileData = new float[_height * _slice];
        for (int j = 0; j < _slice; j++)
        {
            qint64 offset = j * _pixelPerSlice;
            for (int i = 0; i < _height; i++)
            {
                _profileData[j * _height + i] = _originalData.get()[_width * i + _currentProfileSlice + offset];
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

    for (unsigned long i = 0; i < size; i++)
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
        return convertToByte(_topData, _pixelPerSlice, byteImage);
    }
    else if (type == 1)
    {
        return convertToByte(_frontalData, _width * _slice, byteImage);
    }
    else/* if (type == 2)*/
    {
        return convertToByte(_profileData, _height * _slice, byteImage);
    }
}

// Save array to QFile
template <class Type>
void ImageDataTemplate<Type>::saveArray(QFile& file)
{
    file.write((const char*)_topData, sizeof(Type) * _pixelPerSlice);
}

// Rescale array
template <class Type>
void ImageDataTemplate<Type>::rescaleArray(float rescaleSlope, float rescaleIntercept)
{
    if (_topData)
    {
        for (unsigned long i = 0; i < _pixelPerSlice; i++)
        {
            _topData[i] = _originalData.get()[i + _currentTopSlice * _pixelPerSlice] * rescaleSlope + rescaleIntercept;
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
        _topData[i] = _originalData.get()[i + _currentTopSlice * _pixelPerSlice];
    }
    for (int j = 0; j < _slice; j++)
    {
        qint64 offset = j * _pixelPerSlice;
        for (int i = 0; i < _width; i++)
        {
            _frontalData[j * _width + i] = _originalData.get()[i + _currentFrontalSlice * _width + offset];
        }
    }
    for (int j = 0; j < _slice; j++)
    {
        qint64 offset = j * _pixelPerSlice;
        for (int i = 0; i < _height; i++)
        {
            _profileData[j * _height + i] = _originalData.get()[_width * i + _currentProfileSlice + offset];
        }
    }
}

template <class Type>
void ImageDataTemplate<Type>::changeSlice(int type, int slice)
{
    if (type == 0)
    {
        _currentTopSlice = slice;
        for (unsigned long i = 0; i < _pixelPerSlice; i++)
        {
            _topData[i] = _originalData.get()[i + _currentTopSlice * _pixelPerSlice];
        }
    }
    else if (type == 1)
    {
        _currentFrontalSlice = slice;
        for (int j = 0; j < _slice; j++)
        {
            qint64 offset = j * _pixelPerSlice;
            for (int i = 0; i < _width; i++)
            {
                _frontalData[j * _width + i] = _originalData.get()[i + _currentFrontalSlice * _width + offset];
            }
        }
    }
    else/* if (type == 2)*/
    {
        _currentProfileSlice = slice;
        for (int j = 0; j < _slice; j++)
        {
            qint64 offset = j * _pixelPerSlice;
            for (int i = 0; i < _height; i++)
            {
                _profileData[j * _height + i] = _originalData.get()[_width * i + _currentProfileSlice + offset];
            }
        }
    }
}