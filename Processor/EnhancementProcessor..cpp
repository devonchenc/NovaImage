#include "EnhancementProcessor.h"

#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
#include "../Core/GlobalFunc.h"

EnhancementWidget::EnhancementWidget(BaseProcessor* processor, QWidget* parent)
    : ProcessorBaseWidget(processor, parent)
{
    QGroupBox* groupBox = new QGroupBox(tr("Image Enhancement"));

    _laplacianSlider = new QSlider(Qt::Orientation::Horizontal);
    _laplacianSlider->setMinimum(0);
    _laplacianSlider->setMaximum(100);
    _laplacianSlider->setValue(25);
    connect(_laplacianSlider, &QSlider::valueChanged, this, &EnhancementWidget::laplacianValueChanged);
    _laplacianValueLabel = new QLabel("0.5");
    _laplacianValueLabel->setFixedWidth(22);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(new QLabel(tr("Sharpness:")));
    hLayout->addWidget(_laplacianSlider);
    hLayout->addWidget(_laplacianValueLabel);

    _autoCheckBox = new QCheckBox(tr("OTSU Threshold"));
    _autoCheckBox->setChecked(true);
    connect(_autoCheckBox, &QCheckBox::clicked, this, &EnhancementWidget::autoCheckBoxClicked);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(_autoCheckBox);
    vLayout->addLayout(hLayout);

    groupBox->setLayout(vLayout);
    
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(groupBox);
    setLayout(layout);
}

void EnhancementWidget::setOTSUThreshold(int threshold)
{
    _OSTUThreshold = threshold;
    _laplacianSlider->setValue(threshold);
}

void EnhancementWidget::laplacianValueChanged(int value)
{
    float sharpness = value / 50.0f;
    _laplacianValueLabel->setText(QString::number(sharpness));

    emit laplacianChanged(sharpness);
}

void EnhancementWidget::autoCheckBoxClicked()
{
    if (_autoCheckBox->isChecked())
    {
        _laplacianSlider->setValue(_OSTUThreshold);
    }
}

EnhancementProcessor::EnhancementProcessor(QObject* parent)
    : BaseProcessor(true, parent)
    , _laplacianFactor(0.5f)
{

}

EnhancementProcessor::~EnhancementProcessor()
{

}

void EnhancementProcessor::initUI()
{
    EnhancementWidget* widget = new EnhancementWidget(this);
    _processorWidget = widget;
    connect(widget, &EnhancementWidget::laplacianChanged, this, &EnhancementProcessor::laplacianChanged);
    emit createWidget(widget);
}

void EnhancementProcessor::laplacianChanged(float value)
{
    _laplacianFactor = value;

    processForView(getGlobalImage());
}

void EnhancementProcessor::processImage(GeneralImage* srcImage, GeneralImage* dstImage)
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

    for (int j = 1; j < height - 1; j++)
    {
        for (int i = 1; i < width - 1; i++)
        {
            uchar* srcPixel = srcData + j * pitch + i * depth;
            uchar* dstPixel = dstData + j * pitch + i * depth;
            for (int n = 0; n < qMin(depth, 3); n++)
            {
                float laplacianValue = srcPixel[n - pitch - depth] + srcPixel[n - pitch] + srcPixel[n - pitch + depth]
                    + srcPixel[n - depth] + srcPixel[n + depth]
                    + srcPixel[n + pitch - depth] + srcPixel[n + pitch] + srcPixel[n + pitch + depth]
                    - 8 * srcPixel[n];
                laplacianValue = srcPixel[n] - _laplacianFactor * laplacianValue;

                if (laplacianValue > 255)
                {
                    dstPixel[n] = 255;
                }
                else if (laplacianValue < 0)
                {
                    dstPixel[n] = 0;
                }
                else
                {
                    dstPixel[n] = laplacianValue;
                }
            }
        }
    }
}

void EnhancementProcessor::processImage(MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
    uchar* srcByteImage = srcImage->getBYTEImage(width, height);
    uchar* dstByteImage = dstImage->getBYTEImage(width, height);

    float maxValue = srcImage->getMaxValue();
    float minValue = srcImage->getMinValue();
    float variable;
    if (maxValue != minValue)
    {
        variable = 255.0f / (maxValue - minValue);
    }
    else
    {
        variable = 0.0f;
    }

    for (int j = 1; j < height - 1; j++)
    {
        for (int i = 1; i < width - 1; i++)
        {
            int n = j * width + i;
            float laplacianValue = srcImage->getValue(n - width - 1) + srcImage->getValue(n - width) + srcImage->getValue(n - width + 1)
                + srcImage->getValue(n - 1) + srcImage->getValue(n + 1)
                + srcImage->getValue(n + width - 1) + srcImage->getValue(n + width) + srcImage->getValue(n + width + 1)
                - 8 * srcImage->getValue(n);
            laplacianValue = srcImage->getValue(n) - _laplacianFactor * laplacianValue;

            if (laplacianValue > maxValue)
            {
                dstImage->setValue(n, maxValue);
                dstByteImage[3 * n] = dstByteImage[3 * n + 1] = dstByteImage[3 * n + 2] = 255;
            }
            else if (laplacianValue < minValue)
            {
                dstImage->setValue(n, minValue);
                dstByteImage[3 * n] = dstByteImage[3 * n + 1] = dstByteImage[3 * n + 2] = 0;
            }
            else
            {
                dstImage->setValue(n, laplacianValue);
                dstByteImage[3 * n] = dstByteImage[3 * n + 1] = dstByteImage[3 * n + 2] = uchar((laplacianValue - minValue) * variable);
            }
        }
    }

    dstImage->copyByteToImage();
}
