
#include "BaseProcessor.h"

#include <typeinfo>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
//#include "../Image/RegionImage.h"
#include "../Core/GlobalFunc.h"
#include "../Core/Document.h"
#include "../Core/mainwindow.h"

BaseProcessor* BaseProcessor::_currentProcessor = nullptr;

BaseProcessor::BaseProcessor(bool temporary, QObject* parent)
    : QObject(parent)
    , _currentImage(nullptr)
    , _backupImage(nullptr)
    , _temporary(temporary)
{
    if (temporary)
    {
        if (_currentProcessor)
        {
            delete _currentProcessor;
        }
        _currentProcessor = this;
    }

    connect(this, &BaseProcessor::createWidget, getGlobalWindow(), &MainWindow::createProcessorWidget);
}

BaseProcessor::~BaseProcessor()
{
    if (_backupImage)
    {
        delete _backupImage;
        _backupImage = nullptr;
    }
    if (this == _currentProcessor)
    {
        _currentProcessor = nullptr;
    }
}

BaseProcessor* BaseProcessor::getProcessor()
{
    return _currentProcessor;
}

void BaseProcessor::setImage(BaseImage* image)
{
    _currentImage = image;
    if (_backupImage)
    {
        delete _backupImage;
        _backupImage = nullptr;
    }
    if (image)
    {
        _backupImage = image->copyImage();
    }
}

void BaseProcessor::process()
{
    if (_currentImage == nullptr)
        return;

    setCurrentProcessor();

    if (typeid(*_currentImage) == typeid(GeneralImage))
    {
        GeneralImage* generalImage = dynamic_cast<GeneralImage*>(_currentImage);
        processGeneralImage(generalImage);
    }
    else if (dynamic_cast<MonoImage*>(_currentImage))
    {
        MonoImage* monoImage = dynamic_cast<MonoImage*>(_currentImage);
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
    if (_temporary)
    {
        BaseProcessor* oldProcessor = _currentProcessor;
        _currentProcessor = this;
        return oldProcessor;
    }
    else
    {
        return nullptr;
    }
}

void BaseProcessor::apply()
{
    getGlobalDocument()->backup();
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
