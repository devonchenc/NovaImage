#pragma once

#include "GeneralImage.h"
#include "TemplateImage.h"

#define DECLARE_TEMPLATE_INTERFACE \
public: \
	virtual void processUCharImage(TemplateImage<uchar>* image) { processTemplate<uchar>(image); } \
	virtual void processShortImage(TemplateImage<short>* image) { processTemplate<short>(image); } \
	virtual void processUShortImage(TemplateImage<ushort>* image) { processTemplate<ushort>(image); } \
	virtual void processIntImage(TemplateImage<int>* image) { processTemplate<int>(image); } \
	virtual void processUIntImage(TemplateImage<uint>* image) { processTemplate<uint>(image); } \
	virtual void processFloatImage(TemplateImage<float>* image) { processTemplate<float>(image); } \
	virtual void processDoubleImage(TemplateImage<double>* image) { processTemplate<double>(image); }

class BaseImage;
class GeneralImage;
class RegionImage;

// Base class of image processing algorithm
class BaseProcessor
{
	DECLARE_TEMPLATE_INTERFACE

public:
	BaseProcessor();
	~BaseProcessor();

public:
	static BaseProcessor* getProcessor();

	BaseProcessor* setCurrentProcessor();

	// Process image
	void process(BaseImage* pImage);

	// Process float array
	virtual void processArray(float* pArray, int width, int height, float minValue, float maxValue, uchar* pByte)	{}

protected:
	virtual void processGeneralImage(GeneralImage* pImage) {}

//	virtual void ProcessRegionImage(RegionImage* pImage);

	template<typename Type>
	void processTemplate(TemplateImage<Type>* pImage)	{}

	// Convert float data to uchar data
	void convertToByte(float* pArray, int width, int height, float minValue, float maxValue, uchar* pByte);

private:
	static BaseProcessor* _currentProcessor;
};