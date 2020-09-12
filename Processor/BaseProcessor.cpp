
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
    , _srcImage(nullptr)
    , _dstImage(nullptr)
    , _processorWidget(nullptr)
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
    if (_dstImage)
    {
        delete _dstImage;
        _dstImage = nullptr;
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
    if (_srcImage == image)
        return;

    _srcImage = image;
    if (_dstImage)
    {
        delete _dstImage;
        _dstImage = nullptr;
    }
    if (image)
    {
        _dstImage = image->copyImage();
    }
}

void BaseProcessor::process()
{
    if (_srcImage == nullptr)
        return;

    setCurrentProcessor();

/*    if (typeid(*_srcImage) == typeid(GeneralImage))
    {
        GeneralImage* generalImage = dynamic_cast<GeneralImage*>(_srcImage);
        processImage(generalImage, generalImage->getImageEntity());
    }
    else if (dynamic_cast<MonoImage*>(_srcImage))
    {
        MonoImage* monoImage = dynamic_cast<MonoImage*>(_srcImage);
        processImage(monoImage, monoImage->getImageEntity());

        if (getGlobalWindow()->isViewLinked() && monoImage->slice() > 1)
        {
            int viewType = monoImage->viewType();
            if (viewType != 0)
            {
                monoImage->setViewType(0);
                processImageImpl(monoImage, monoImage->getImageEntity());
            }
            if (viewType != 1)
            {
                monoImage->setViewType(1);
                processImageImpl(monoImage, monoImage->getImageEntity());
            }
            if (viewType != 2)
            {
                monoImage->setViewType(2);
                processImageImpl(monoImage, monoImage->getImageEntity());
            }
            monoImage->setViewType(viewType);
        }
    }
//   	else if (typeid(*pImage) == typeid(RegionImage))
//    {
//        RegionImage* pRegionImage = dynamic_cast<RegionImage*>(pImage);
//        assert(pRegionImage);
//        ProcessRegionImage(pRegionImage);
//    }
    getGlobalDocument()->backup();*/
}

void BaseProcessor::processForView(BaseImage* image)
{
    setImage(image);

    setCurrentProcessor();

    if (typeid(*image) == typeid(GeneralImage))
    {
        GeneralImage* generalImage = dynamic_cast<GeneralImage*>(image);
        GeneralImage* dstImage = dynamic_cast<GeneralImage*>(_dstImage);
        processImage(generalImage, dstImage);

        repaintView(_dstImage->getImageEntity());
    }
    else if (dynamic_cast<MonoImage*>(image))
    {
        MonoImage* monoImage = dynamic_cast<MonoImage*>(image);
        MonoImage* dstImage = dynamic_cast<MonoImage*>(_dstImage);
        processImage(monoImage, dstImage);

        repaintView(_dstImage->getImageEntity(), 0);

        if (getGlobalWindow()->isViewLinked() && monoImage->slice() > 1)
        {
            int viewType = monoImage->viewType();
            if (viewType != 0)
            {
                monoImage->setViewType(0);
                processImage(monoImage, dstImage);
                repaintView(_dstImage->getImageEntity(), 0);
            }
            if (viewType != 1)
            {
                monoImage->setViewType(1);
                processImage(monoImage, dstImage);
                repaintView(_dstImage->getImageEntity(), 1);
            }
            if (viewType != 2)
            {
                monoImage->setViewType(2);
                processImage(monoImage, dstImage);
                repaintView(_dstImage->getImageEntity(), 2);
            }
            monoImage->setViewType(viewType);
        }
    }
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
    if (_srcImage == nullptr || _dstImage == nullptr)
        return;

    *_srcImage = *_dstImage;

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
