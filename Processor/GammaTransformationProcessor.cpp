#include "GammaTransformationProcessor.h"

#include <cmath>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
#include "../Core/GlobalFunc.h"

GammaTransformationWidget::GammaTransformationWidget(BaseProcessor* processor, QWidget* parent)
    : ProcessorBaseWidget(processor, parent)
{
    QGroupBox* groupBox = new QGroupBox(tr("Gamma Transformation"));

    _gammaLabel = new QLabel(tr("Gamma"));
    _gammaSlider = new QSlider(Qt::Orientation::Horizontal);
    _gammaSlider->setMinimum(-6);
    _gammaSlider->setMaximum(6);
    connect(_gammaSlider, &QSlider::valueChanged, this, &GammaTransformationWidget::gammaValueChanged);
    _gammaValueLabel = new QLabel("1");
    _gammaValueLabel->setFixedWidth(26);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(_gammaLabel);
    hLayout->addWidget(_gammaSlider);
    hLayout->addWidget(_gammaValueLabel);

    groupBox->setLayout(hLayout);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(groupBox);
    setLayout(layout);
}

void GammaTransformationWidget::reset()
{
    _gammaSlider->setValue(0);
}

void GammaTransformationWidget::gammaValueChanged(int value)
{
    float gamma;
    switch (value)
    {
    case -6:
        gamma = 0.04f;
        break;
    case -5:
        gamma = 0.1f;
        break;
    case -4:
        gamma = 0.2f;
        break;
    case -3:
        gamma = 0.33f;
        break;
    case -2:
        gamma = 0.5f;
        break;
    case -1:
        gamma = 0.67f;
        break;
    case 0:
        gamma = 1.0f;
        break;
    case 1:
        gamma = 1.5f;
        break;
    case 2:
        gamma = 2.0f;
        break;
    case 3:
        gamma = 3.0f;
        break;
    case 4:
        gamma = 5.0f;
        break;
    case 5:
        gamma = 10.0f;
        break;
    case 6:
        gamma = 25.0f;
        break;
    }
    _gammaValueLabel->setText(QString::number(gamma));

    emit parametersChanged(gamma);
}

GammaTransformationProcessor::GammaTransformationProcessor(QObject* parent)
    : BaseProcessor(false, parent)
    , _gamma(1)
{

}

GammaTransformationProcessor::~GammaTransformationProcessor()
{

}

void GammaTransformationProcessor::initUI()
{
    GammaTransformationWidget* widget = new GammaTransformationWidget(this);
    _processorWidget = widget;
    connect(widget, &GammaTransformationWidget::parametersChanged, this, &GammaTransformationProcessor::setGamma);
    emit createWidget(_processorWidget);
}

void GammaTransformationProcessor::processImage(GeneralImage* srcImage, GeneralImage* dstImage)
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

            *(dstPixel) = round(pow(*srcPixel / 255.0f, _gamma) * 255.0f);
        }
    }
}

void GammaTransformationProcessor::processImage(MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
    uchar* byteImage = dstImage->getBYTEImage(width, height);

    float maxValue = srcImage->getMaxValue();
    float minValue = srcImage->getMinValue();
    float gap = maxValue - minValue;
    float variable;
    if (maxValue != minValue)
    {
        variable = 255.0f / gap;
    }
    else
    {
        variable = 0.0f;
    }

    for (int i = 0; i < width * height; i++)
    {
        float dstValue = pow((srcImage->getValue(i) - minValue) / gap, _gamma) * gap + minValue;
        dstImage->setValue(i, dstValue);
        byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = uchar((dstValue - minValue) * variable);
    }

    // Copy to image
    dstImage->copyByteToImage();
}

void GammaTransformationProcessor::setGamma(float gamma)
{
    _gamma = gamma;

    processForView(getGlobalImage());
}
