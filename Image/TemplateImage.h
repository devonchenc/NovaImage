#pragma once

#include "BaseImage.h"
#include <QMessageBox>
#include <cmath>
#include <iostream>
#include "../GlobalFunc.h"

#ifdef _MSC_VER
#pragma execution_character_set("UTF-8")
#endif

template <class Type>
class TemplateImage : public BaseImage
{
public:
	TemplateImage();
	TemplateImage(const QString& pathName, int width, int height);
	virtual ~TemplateImage();

public:
	// Get original data pointer
	Type* getOriginalData();

	// Get processing data pointer
	Type* getProcessingData();

	// Find top and bottom value in data
	bool findTopAndBottom(Type* pData, int num);

	Type getMinimumValue() const { return _minValue; }

	void setMinimumValue(Type minValue) { _minValue = minValue; }

	Type getMaximumValue()	const { return _maxValue; }

	void setMaximumValue(Type maxValue) { _maxValue = maxValue; }

	// get uchar data
	uchar* getBYTEImage() { return _byteImage; }

	// Allocate memory
	bool allocateMemory();

	// Convert float data to uchar data
	bool convertToByte();

	// Copy to image
	bool copyToImage();

public:
	// Histogram statistic
	virtual void histogramStatistic();

	virtual float getValue(const QPoint& position) const;

	virtual int slice() const { return _slice; }

	virtual float getMinValue() { return float(_minValue); }

	virtual float getMaxValue() { return float(_maxValue); }

protected:
	TemplateImage(const QString& pathName);

protected:
	Type* _originalData;

	Type* _processingData;

	Type _minValue, _maxValue;

	uchar* _byteImage;

	int _slice;

	int _currentSlice;
};

template <class Type>
TemplateImage<Type>::TemplateImage()
	: _originalData(nullptr)
	, _processingData(nullptr)
	, _byteImage(nullptr)
{

}

template <class Type>
TemplateImage<Type>::TemplateImage(const QString& pathName, int width, int height)
	: BaseImage(pathName)
	, _processingData(nullptr)
	, _byteImage(nullptr)
{
	_width = width;
	_height = height;
	_slice = 1;
	_originalData = new Type[_width * _height * _slice];
}

template <class Type>
TemplateImage<Type>::TemplateImage(const QString& pathName)
	: BaseImage(pathName)
	, _originalData(nullptr)
	, _processingData(nullptr)
	, _byteImage(nullptr)
{

}

template <class Type>
TemplateImage<Type>::~TemplateImage()
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
	if (_byteImage)
	{
		delete [] _byteImage;
		_byteImage = nullptr;
	}
}

// Get original data pointer
template <typename Type>
Type* TemplateImage<Type>::getOriginalData()
{
	return _originalData;
}

// Get processing data pointer
template <typename Type>
Type* TemplateImage<Type>::getProcessingData()
{
	return _processingData;
}

// Traverse the max and min values in array
template <class Type>
bool TemplateImage<Type>::findTopAndBottom(Type* pData, int num)
{
	Q_ASSERT(pData);

	_minValue = _maxValue = pData[0];
	for (int i = 1; i < num; i++)
	{
		if ((std::is_same<Type, float>::value || std::is_same<Type, double>::value) && (std::isnan(pData[i]) || std::isinf(pData[i])))
		{
			QMessageBox::critical(nullptr, "Error in traversing data", "Invalid value in data!", QMessageBox::Ok);
			return false;
		}

		if (_minValue > pData[i])
		{
			_minValue = pData[i];
		}
		if (_maxValue < pData[i])
		{
			_maxValue = pData[i];
		}
	}

	return true;
}

// Histogram statistic
template <class Type>
void TemplateImage<Type>::histogramStatistic()
{
	memset(_grayPixelNumber, 0, sizeof(uint) * 256);
	memset(_redPixelNumber, 0, sizeof(uint) * 256);
	memset(_greenPixelNumber, 0, sizeof(uint) * 256);
	memset(_bluePixelNumber, 0, sizeof(uint) * 256);

	float minValue = getMinimumValue();
	float maxValue = getMaximumValue();
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

// Allocate memory
template <class Type>
bool TemplateImage<Type>::allocateMemory()
{
	try
	{
		_processingData = new Type[_width * _height];
		memcpy(_processingData, _originalData, sizeof(Type) * _width * _height);

		// 3通道
		_byteImage = new uchar[_width * _height * 3];

		_pImage = new QImage(_width, _height, QImage::Format_RGB888);
	}
	catch (const std::bad_alloc& e)
	{
		std::cout << "Allocation failed: " << e.what() << std::endl;
	}

	return true;
}

// Convert data to byte
template <class Type>
bool TemplateImage<Type>::convertToByte()
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

	for (int i = 0; i < _width * _height; i++)
	{
		_byteImage[3 * i] = _byteImage[3 * i + 1] = _byteImage[3 * i + 2] =
				uchar((_processingData[i] - _minValue) * variable);
	}

	return true;
}

// 拷贝到图像
template <class Type>
bool TemplateImage<Type>::copyToImage()
{
	return copyByteToImage(_byteImage, _width, _height, _pImage);
}

template <class Type>
float TemplateImage<Type>::getValue(const QPoint& position) const
{
    int index = position.y() * _width + position.x();
    return float(_processingData[index]);
}