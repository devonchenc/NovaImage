#include "ThresholdSegmentationProcessor.h"

#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
#include "../Core/GlobalFunc.h"

ThresholdSegmentationWidget::ThresholdSegmentationWidget(BaseProcessor* processor, QWidget* parent)
    : ProcessorBaseWidget(processor, parent)
{
    QGroupBox* groupBox = new QGroupBox(tr("Threshold Segmentation"));

    _thresholdSlider = new QSlider(Qt::Orientation::Horizontal);
    _thresholdSlider->setMinimum(0);
    _thresholdSlider->setMaximum(255);
    connect(_thresholdSlider, &QSlider::valueChanged, this, &ThresholdSegmentationWidget::valueChanged);
    _thresholdValueLabel = new QLabel;
    _thresholdValueLabel->setFixedWidth(20);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(new QLabel(tr("Threshold:")));
    hLayout->addWidget(_thresholdSlider);
    hLayout->addWidget(_thresholdValueLabel);

    _autoCheckBox = new QCheckBox(tr("OTSU Threshold"));
    _autoCheckBox->setChecked(true);
    connect(_autoCheckBox, &QCheckBox::clicked, this, &ThresholdSegmentationWidget::autoCheckBoxClicked);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(_autoCheckBox);
    vLayout->addLayout(hLayout);

    groupBox->setLayout(vLayout);
    
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(groupBox);
    setLayout(layout);
}

void ThresholdSegmentationWidget::setOTSUThreshold(int threshold)
{
    _OSTUThreshold = threshold;
    _thresholdSlider->setValue(threshold);
}

void ThresholdSegmentationWidget::valueChanged(int value)
{
    _thresholdValueLabel->setText(QString::number(value));

    _autoCheckBox->setChecked(value == _OSTUThreshold);

    emit thresholdChanged(value);
}

void ThresholdSegmentationWidget::autoCheckBoxClicked()
{
    if (_autoCheckBox->isChecked())
    {
        _thresholdSlider->setValue(_OSTUThreshold);
    }
}

ThresholdSegmentationProcessor::ThresholdSegmentationProcessor(QObject* parent)
    : BaseProcessor(true, parent)
    , _threshold(128)
{

}

void ThresholdSegmentationProcessor::initUI()
{
    ThresholdSegmentationWidget* widget = new ThresholdSegmentationWidget(this);
    _processorWidget = widget;
    connect(widget, &ThresholdSegmentationWidget::thresholdChanged, this, &ThresholdSegmentationProcessor::thresholdChanged);
    emit createWidget(widget);

    if (_srcImage)
    {
        int width = _srcImage->width();
        int height = _srcImage->height();
        if (dynamic_cast<MonoImage*>(_srcImage))
        {
            MonoImage* monoImage = dynamic_cast<MonoImage*>(_srcImage);
            monoImage->getBYTEImage(width, height);
        }
        int threshold = findOtsuThreshold(_srcImage->getGrayPixelArray(), width * height);
        widget->setOTSUThreshold(threshold);
    }
}

void ThresholdSegmentationProcessor::thresholdChanged(int value)
{
    _threshold = value;

    processForView(getGlobalImage());
}

void ThresholdSegmentationProcessor::processImage(const GeneralImage* srcImage, GeneralImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width = srcImage->width();
    int height = srcImage->height();
    const QImage* imageEntity = srcImage->getImageEntity();
    const uchar* srcData = imageEntity->bits();
    uchar* dstData = dstImage->getImageEntity()->bits();
    int pitch = imageEntity->bytesPerLine();
    int depth = imageEntity->depth() / 8;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            uchar* dstPixel = dstData + j * pitch + i * depth;
            const uchar* srcPixel = srcData + j * pitch + i * depth;
            for (int n = 0; n < qMin(depth, 3); n++)
            {
                if (srcPixel[n] > _threshold)
                {
                    dstPixel[n] = 255;
                }
                else
                {
                    dstPixel[n] = 0;
                }
            }
        }
    }
}

void ThresholdSegmentationProcessor::processImage(const MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
    const uchar* srcByteImage = srcImage->getBYTEImage(width, height);
    uchar* dstByteImage = dstImage->getBYTEImage(width, height);

    float maxValue = srcImage->getMaxValue();
    float minValue = srcImage->getMinValue();

    for (int i = 0; i < width * height; i++)
    {
        if (srcByteImage[3 * i] > _threshold)
        {
            dstImage->setValue(i, maxValue);
            dstByteImage[3 * i] = dstByteImage[3 * i + 1] = dstByteImage[3 * i + 2] = 255;
        }
        else
        {
            dstImage->setValue(i, minValue);
            dstByteImage[3 * i] = dstByteImage[3 * i + 1] = dstByteImage[3 * i + 2] = 0;
        }
    }

    dstImage->copyByteToImage();
}

int ThresholdSegmentationProcessor::findOtsuThreshold(uint* grayPixelArray, int pixelNumber)
{
    const int grayscale = 256;

    float* pixelP = new float[grayscale];
    float* sumP = new float[grayscale];
    float* sumValue = new float[grayscale];
    float tempSumP = 0;
    float tempSumValue = 0;
    for (int i = 0; i < grayscale; i++)
    {
        // Calculate the probability of each gray level
        pixelP[i] = float(grayPixelArray[i]) / float(pixelNumber);
        // Calculate the sum of probabilities
        tempSumP += pixelP[i];
        sumP[i] = tempSumP;
        // 
        tempSumValue += i * pixelP[i];
        sumValue[i] = tempSumValue;
    }

    int threshold = 0;
    float maxVar = 0;
    for (int i = 0; i < grayscale; i++)
    {
        if (sumP[i] > 0)
        {
            float var = (sumValue[grayscale - 1] * sumP[i] - sumValue[i]) * (sumValue[grayscale - 1] * sumP[i] - sumValue[i]) / (sumP[i] * (1 - sumP[i]));
            if (var > maxVar)
            {
                maxVar = var;
                threshold = i;
            }
        }
    }

    delete[] pixelP;
    delete[] sumP;
    delete[] sumValue;

    return threshold;
}
