#include "BrightnessAndContrastProcessor.h"

#include <cmath>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
#include "../Core/GlobalFunc.h"

BrightnessAndContrastWidget::BrightnessAndContrastWidget(BaseProcessor* processor, QWidget* parent)
    : ProcessorBaseWidget(processor, parent)
{
    QGroupBox* groupBox = new QGroupBox(tr("Brightness/Contrast"));

    _brightnessLabel = new QLabel(tr("Bright"));
    _contrastLabel = new QLabel(tr("Contrast"));
    _brightnessSlider = new QSlider(Qt::Orientation::Horizontal);
    _brightnessSlider->setMinimum(-100);
    _brightnessSlider->setMaximum(100);
    connect(_brightnessSlider, &QSlider::valueChanged, this, &BrightnessAndContrastWidget::brightnessValueChanged);
    _contrastSlider = new QSlider(Qt::Orientation::Horizontal);
    _contrastSlider->setMinimum(-100);
    _contrastSlider->setMaximum(100);
    connect(_contrastSlider, &QSlider::valueChanged, this, &BrightnessAndContrastWidget::contrastValueChanged);
    _brightnessValueLabel = new QLabel("0");
    _brightnessValueLabel->setFixedWidth(20);
    _contrastValueLabel = new QLabel("0");
    _contrastValueLabel->setFixedWidth(20);

    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget(_brightnessLabel, 0, 0);
    gridLayout->addWidget(_brightnessSlider, 0, 1);
    gridLayout->addWidget(_brightnessValueLabel, 0, 2);
    gridLayout->addWidget(_contrastLabel, 1, 0);
    gridLayout->addWidget(_contrastSlider, 1, 1);
    gridLayout->addWidget(_contrastValueLabel, 1, 2);

    groupBox->setLayout(gridLayout);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(groupBox);
    setLayout(layout);
}

void BrightnessAndContrastWidget::reset()
{
    _brightnessSlider->setValue(0);
    _contrastSlider->setValue(0);
}

void BrightnessAndContrastWidget::brightnessValueChanged(int value)
{
    _brightnessValueLabel->setText(QString::number(value));

    emit parametersChanged(value, _contrastSlider->value());
}

void BrightnessAndContrastWidget::contrastValueChanged(int value)
{
    _contrastValueLabel->setText(QString::number(value));

    emit parametersChanged(_brightnessSlider->value(), value);
}

BrightnessAndContrastProcessor::BrightnessAndContrastProcessor(QObject* parent)
    : BaseProcessor(false, parent)
    , _brightness(0)
    , _contrast(0)
{

}

void BrightnessAndContrastProcessor::initUI()
{
    BrightnessAndContrastWidget* widget = new BrightnessAndContrastWidget(this);
    _processorWidget = widget;
    connect(widget, &BrightnessAndContrastWidget::parametersChanged, this, &BrightnessAndContrastProcessor::setBrightnessAndContrast);
    emit createWidget(_processorWidget);
}

void BrightnessAndContrastProcessor::processImage(GeneralImage* srcImage, GeneralImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width = srcImage->width();
    int height = srcImage->height();
    QImage* imageEntity = srcImage->getImageEntity();
    uchar* srcData = imageEntity->bits();
    uchar* dstData = dstImage->getImageEntity()->bits();
    int pitch = imageEntity->bytesPerLine();
    int depth = imageEntity->depth() / 8;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width * depth; i++)
        {
            uchar* dstPixel = dstData + j * pitch + i;
            uchar* srcPixel = srcData + j * pitch + i;
            float result = (*srcPixel - 127.0f) * (_contrast + 100) / 100.0f + 127.0f;
            result *= (100.0f + _brightness / 2.0f) / 100.0f;
            if (result >= 255)
            {
                *(dstPixel) = 255;
            }
            else if (result <= 0)
            {
                *(dstPixel) = 0;
            }
            else
            {
                *(dstPixel) = round(result);
            }
        }
    }
}

void BrightnessAndContrastProcessor::processImage(MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
    uchar* byteImage = dstImage->getBYTEImage(width, height);

    float maxValue = srcImage->getMaxValue();
    float minValue = srcImage->getMinValue();

    float average = float(minValue + maxValue) / 2.0f;
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
        float dstValue = float(srcImage->getValue(i) - average) * (_contrast + 100) / 100.0f + average;
        dstValue *= (100.0f + _brightness / 2.0f) / 100.0f;
        if (dstValue >= maxValue)
        {
            dstImage->setValue(i, maxValue);
            byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 255;
        }
        else if (dstValue <= minValue)
        {
            dstImage->setValue(i, minValue);
            byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 0;
        }
        else
        {
            dstImage->setValue(i, dstValue);
            byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = uchar((dstValue - minValue) * variable);
        }
    }

    // Copy to image
    dstImage->copyByteToImage();
}

void BrightnessAndContrastProcessor::setBrightnessAndContrast(int brightness, int contrast)
{
    _brightness = brightness;
    _contrast = contrast;

    processForView(getGlobalImage());
}
