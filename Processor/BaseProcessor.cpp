
#include "BaseProcessor.h"

#include <typeinfo>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
//#include "../Image/RegionImage.h"
#include "../Core/GlobalFunc.h"
#include "../Core/Document.h"
#include "../Core/mainwindow.h"

BaseProcessor* BaseProcessor::_currentProcessor = nullptr;

BaseProcessor::BaseProcessor()
    : _image(nullptr)
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

void BaseProcessor::process()
{
    setCurrentProcessor();

    if (typeid(*_image) == typeid(GeneralImage))
    {
        GeneralImage* generalImage = dynamic_cast<GeneralImage*>(_image);
        processGeneralImage(generalImage);
    }
    else if (dynamic_cast<MonoImage*>(_image))
    {
        MonoImage* monoImage = dynamic_cast<MonoImage*>(_image);
        processMonoImage(monoImage);

        if (getGlobalWindow()->isViewLinked() && monoImage->slice() > 1)
        {
            int viewType = monoImage->viewType();
            if (viewType != 0)
            {
                monoImage->setViewType(0);
                processMonoImage(monoImage);
            }
            if (viewType != 1)
            {
                monoImage->setViewType(1);
                processMonoImage(monoImage);
            }
            if (viewType != 2)
            {
                monoImage->setViewType(2);
                processMonoImage(monoImage);
            }
            monoImage->setViewType(viewType);
        }
    }
    /*	else if (typeid(*pImage) == typeid(RegionImage))
    {
        RegionImage* pRegionImage = dynamic_cast<RegionImage*>(pImage);
        assert(pRegionImage);
        ProcessRegionImage(pRegionImage);
    }*/

    getGlobalDocument()->backup();
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
