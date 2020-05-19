
#include "BaseProcessor.h"

#include <typeinfo.h>
#include "GeneralImage.h"
//#include "FloatScanImage.h"
//#include "ShortScanImage.h"
//#include "RegionImage.h"

BaseProcessor* BaseProcessor::_currentProcessor = nullptr;

BaseProcessor::BaseProcessor()
{
	_currentProcessor = this;
}

BaseProcessor::~BaseProcessor()
{

}

BaseProcessor* BaseProcessor::getProcessor()
{
	return _currentProcessor;
}

void BaseProcessor::process(BaseImage* pImage)
{
	setCurrentProcessor();
	
	if (typeid(*pImage) == typeid(GeneralImage))
	{
		GeneralImage* pGeneralImage = dynamic_cast<GeneralImage*>(pImage);
		processGeneralImage(pGeneralImage);
	}
	else if (dynamic_cast<TemplateImage<float>*>(pImage))
	{
		TemplateImage<float>* pFloatImage = dynamic_cast<TemplateImage<float>*>(pImage);
		assert(pFloatImage);
		processFloatImage(pFloatImage);
	}
	else if (dynamic_cast<TemplateImage<ushort>*>(pImage))
	{
		TemplateImage<ushort>* pUShortImage = dynamic_cast<TemplateImage<ushort>*>(pImage);
		assert(pUShortImage);
		processUShortImage(pUShortImage);
	}
	else if (dynamic_cast<TemplateImage<uchar>*>(pImage))
	{
		TemplateImage<uchar>* pUCharImage = dynamic_cast<TemplateImage<uchar>*>(pImage);
		assert(pUCharImage);
		processUCharImage(pUCharImage);
	}
	else if (dynamic_cast<TemplateImage<short>*>(pImage))
	{
		TemplateImage<short>* pShortImage = dynamic_cast<TemplateImage<short>*>(pImage);
		assert(pShortImage);
		processShortImage(pShortImage);
	}
	else if (dynamic_cast<TemplateImage<int>*>(pImage))
	{
		TemplateImage<int>* pIntImage = dynamic_cast<TemplateImage<int>*>(pImage);
		assert(pIntImage);
		processIntImage(pIntImage);
	}
	else if (dynamic_cast<TemplateImage<unsigned int>*>(pImage))
	{
		TemplateImage<unsigned int>* pUIntImage = dynamic_cast<TemplateImage<unsigned int>*>(pImage);
		assert(pUIntImage);
		processUIntImage(pUIntImage);
	}
	else if (dynamic_cast<TemplateImage<double>*>(pImage))
	{
		TemplateImage<double>* pDoubleImage = dynamic_cast<TemplateImage<double>*>(pImage);
		assert(pDoubleImage);
		processDoubleImage(pDoubleImage);
	}
/*	else if (typeid(*pImage) == typeid(RegionImage))
	{
		RegionImage* pRegionImage = dynamic_cast<RegionImage*>(pImage);
		assert(pRegionImage);
		ProcessRegionImage(pRegionImage);
	}*/
}
/* TODO
void BaseProcessor::ProcessRegionImage(RegionImage* pImage)
{
	RegionImage* pRegionImage = dynamic_cast<RegionImage*>(pImage);
	assert(pRegionImage);

	Process(pRegionImage->GetImplImage());
}*/

BaseProcessor* BaseProcessor::setCurrentProcessor()
{
	BaseProcessor* oldProcessor = _currentProcessor;
	_currentProcessor = this;
	return oldProcessor;
}

void BaseProcessor::convertToByte(float* pArray, int width, int height, float minValue, float maxValue, uchar* pByte)
{
	assert(pArray && pByte);

	float variable;
	if (maxValue != minValue)
	{
		variable = 255.0f / float(maxValue - minValue);
	}
	else
	{
		variable = 0.0f;
	}

	for (int i = 0; i < width * height; i++)
	{
		pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = uchar((pArray[i] - minValue) * variable);
	}
}