
#include "BaseProcessor.h"

#include <typeinfo>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
//#include "../Image/RegionImage.h"
#include "../Core/GlobalFunc.h"
#include "../Core/Document.h"
#include "../Core/mainwindow.h"
#include "../Widget/ProcessorBaseWidget.h"

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
        if (_currentProcessor && _currentProcessor->isTemporary())
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

BaseProcessor* BaseProcessor::currentProcessor()
{
    return _currentProcessor;
}

void BaseProcessor::setImage(BaseImage* image)
{
    if (_srcImage == image && _dstImage)
    {
        if (_dstImage->width() == image->width() && _dstImage->height() == image->height() && _dstImage->slice() == image->slice())
            return;
    }

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
//      else if (typeid(*pImage) == typeid(RegionImage))
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
        GeneralImage* srcImage = dynamic_cast<GeneralImage*>(image);
        GeneralImage* dstImage = dynamic_cast<GeneralImage*>(_dstImage);
        processImage(srcImage, dstImage);

        repaintView(_dstImage->getImageEntity());
    }
    else if (dynamic_cast<MonoImage*>(image))
    {
        MonoImage* srcImage = dynamic_cast<MonoImage*>(image);
        MonoImage* dstImage = dynamic_cast<MonoImage*>(_dstImage);

        int viewType = srcImage->viewType();
        dstImage->setViewType(viewType);
        processImage(srcImage, dstImage);

        repaintView(dstImage->getImageEntity(), viewType);

        if (getGlobalWindow()->isViewLinked() && srcImage->slice() > 1)
        {
            if (viewType != 0)
            {
                srcImage->setViewType(0);
                dstImage->setViewType(0);
                processImage(srcImage, dstImage);
                repaintView(dstImage->getImageEntity(), 0);
            }
            if (viewType != 1)
            {
                srcImage->setViewType(1);
                dstImage->setViewType(1);
                processImage(srcImage, dstImage);
                repaintView(dstImage->getCoronalImage().get(), 1);
            }
            if (viewType != 2)
            {
                srcImage->setViewType(2);
                dstImage->setViewType(2);
                processImage(srcImage, dstImage);
                repaintView(dstImage->getSagittalImage().get(), 2);
            }
            srcImage->setViewType(viewType);
            dstImage->setViewType(viewType);
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
    BaseProcessor* oldProcessor = _currentProcessor;
    _currentProcessor = this;
    return oldProcessor;
}

void BaseProcessor::apply()
{
    if (_srcImage == nullptr || _dstImage == nullptr)
        return;

    _dstImage->copyToImage(_srcImage);

    if (_processorWidget)
    {
        _processorWidget->reset();
    }

    getGlobalDocument()->backup();
}

void BaseProcessor::convertToByte(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
{
    assert(array && pByte);

    float variable;
    if (maxValue != minValue)
    {
        variable = 255.0f / (maxValue - minValue);
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
