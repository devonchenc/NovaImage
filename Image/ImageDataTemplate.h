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
	ImageDataTemplate(unsigned long pixelCount);
	virtual ~ImageDataTemplate();

public:
	// Get original data pointer
	void* getOriginalData() override
	{
		return static_cast<void*>(_originalData);
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

protected:
	Type* _originalData;

	float* _processingData;

	int _slice;

	int _currentSlice;
};

template <class Type>
ImageDataTemplate<Type>::ImageDataTemplate(unsigned long pixelCount)
	: ImageData(pixelCount)
	, _processingData(nullptr)
{
	_originalData = new Type[_pixelCount];
}

template <class Type>
ImageDataTemplate<Type>::~ImageDataTemplate()
{
	if (_originalData)
	{
		delete [] _originalData;
		_originalData = nullptr;
	}
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
	_minValue = _maxValue = _originalData[0];
	for (int i = 1; i < _pixelCount; i++)
	{
		if ((std::is_same<Type, float>::value || std::is_same<Type, double>::value) && (std::isnan(_originalData[i]) || std::isinf(_originalData[i])))
		{
			QMessageBox::critical(nullptr, "Error in traversing data", "Invalid value in data!", QMessageBox::Ok);
			return false;
		}

		if (_minValue > _originalData[i])
		{
			_minValue = _originalData[i];
		}
		if (_maxValue < _originalData[i])
		{
			_maxValue = _originalData[i];
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
		_processingData = new float[_pixelCount];
		for (int i = 0; i < _pixelCount; i++)
		{
			_processingData[i] = _originalData[i];
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

	for (int i = 0; i < _pixelCount; i++)
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
	file.write((const char*)_originalData, sizeof(Type) * _pixelCount);
}

// Rescale array
template <class Type>
void ImageDataTemplate<Type>::rescaleArray(float rescaleSlope, float rescaleIntercept)
{
	if (_processingData)
	{
		for (int i = 0; i < _pixelCount; i++)
		{
			_processingData[i] = _originalData[i] * rescaleSlope + rescaleIntercept;
		}

		_minValue = _minValue * rescaleSlope + rescaleIntercept;
		_maxValue = _maxValue * rescaleSlope + rescaleIntercept;
	}
}