#pragma once

typedef unsigned char uchar;

class ImageData
{
public:
	ImageData();

	ImageData(unsigned long pixelCount);

	virtual ~ImageData();

public:
	// Get original data pointer
	virtual void* getOriginalData() = 0;

	// Get processing data pointer
	virtual void* getProcessingData() = 0;

	// Get pixel value of processing data
	virtual float getProcessingValue(int index) = 0;

	// Allocate memory
	virtual bool allocateMemory();

	// Find top and bottom value in data
	virtual bool findTopAndBottom() = 0;

	// Allocate data to byte
	virtual bool convertToByte() = 0;

public:
	float getMinimumValue() const { return _minValue; }

	void setMinimumValue(float minValue) { _minValue = minValue; }

	float getMaximumValue()	const { return _maxValue; }

	void setMaximumValue(float maxValue) { _maxValue = maxValue; }

	// get uchar data
	uchar* getBYTEImage() { return _byteImage; }

protected:
	uchar* _byteImage;

	unsigned long _pixelCount;

	float _minValue, _maxValue;
};