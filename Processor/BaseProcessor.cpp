
#include "BaseProcessor.h"

#include <typeinfo.h>
#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
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

void BaseProcessor::process(BaseImage* image)
{
	setCurrentProcessor();
	
	if (typeid(*image) == typeid(GeneralImage))
	{
		GeneralImage* generalImage = dynamic_cast<GeneralImage*>(image);
		processGeneralImage(generalImage);
	}
	else if (dynamic_cast<MonoImage*>(image))
	{
		MonoImage* monoImage = dynamic_cast<MonoImage*>(image);
		processMonoImage(monoImage);
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

void BaseProcessor::convertToByte(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
{
	assert(array && pByte);

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
		pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = uchar((array[i] - minValue) * variable);
	}
}