
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
/*	else if (dynamic_cast<ImageDataTemplate<float>*>(pImage))
	{
		ImageDataTemplate<float>* pFloatImage = dynamic_cast<ImageDataTemplate<float>*>(pImage);
		assert(pFloatImage);
		processFloatImage(pFloatImage);
	}
	else if (dynamic_cast<ImageDataTemplate<ushort>*>(pImage))
	{
		ImageDataTemplate<ushort>* pUShortImage = dynamic_cast<ImageDataTemplate<ushort>*>(pImage);
		assert(pUShortImage);
		processUShortImage(pUShortImage);
	}
	else if (dynamic_cast<ImageDataTemplate<uchar>*>(pImage))
	{
		ImageDataTemplate<uchar>* pUCharImage = dynamic_cast<ImageDataTemplate<uchar>*>(pImage);
		assert(pUCharImage);
		processUCharImage(pUCharImage);
	}
	else if (dynamic_cast<ImageDataTemplate<short>*>(pImage))
	{
		ImageDataTemplate<short>* pShortImage = dynamic_cast<ImageDataTemplate<short>*>(pImage);
		assert(pShortImage);
		processShortImage(pShortImage);
	}
	else if (dynamic_cast<ImageDataTemplate<int>*>(pImage))
	{
		ImageDataTemplate<int>* pIntImage = dynamic_cast<ImageDataTemplate<int>*>(pImage);
		assert(pIntImage);
		processIntImage(pIntImage);
	}
	else if (dynamic_cast<ImageDataTemplate<unsigned int>*>(pImage))
	{
		ImageDataTemplate<unsigned int>* pUIntImage = dynamic_cast<ImageDataTemplate<unsigned int>*>(pImage);
		assert(pUIntImage);
		processUIntImage(pUIntImage);
	}
	else if (dynamic_cast<ImageDataTemplate<double>*>(pImage))
	{
		ImageDataTemplate<double>* pDoubleImage = dynamic_cast<ImageDataTemplate<double>*>(pImage);
		assert(pDoubleImage);
		processDoubleImage(pDoubleImage);
	}*/
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