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
    , _sobelFactor(0.2f)
    , _laplacianFactor(0.5f)
{
    QGroupBox* groupBox = new QGroupBox(tr("Image Enhancement"));

    _sobelCheckBox = new QCheckBox(tr("Sobel"));
    _sobelCheckBox->setChecked(true);
    connect(_sobelCheckBox, &QCheckBox::clicked, this, &EnhancementWidget::sobelCheckBoxClicked);

    _sobelSlider = new QSlider(Qt::Orientation::Horizontal);
    _sobelSlider->setMinimum(0);
    _sobelSlider->setMaximum(100);
    _sobelSlider->setValue(20);
    connect(_sobelSlider, &QSlider::valueChanged, this, &EnhancementWidget::sobelValueChanged);
    _sobelValueLabel = new QLabel("0.2");
    _sobelValueLabel->setFixedWidth(25);

    QHBoxLayout* h1Layout = new QHBoxLayout;
    h1Layout->addWidget(new QLabel(tr("Sharpness:")));
    h1Layout->addWidget(_sobelSlider);
    h1Layout->addWidget(_sobelValueLabel);

    _laplacianCheckBox = new QCheckBox(tr("Laplacian"));
    _laplacianCheckBox->setChecked(true);
    connect(_laplacianCheckBox, &QCheckBox::clicked, this, &EnhancementWidget::laplacianCheckBoxClicked);

    _laplacianSlider = new QSlider(Qt::Orientation::Horizontal);
    _laplacianSlider->setMinimum(0);
    _laplacianSlider->setMaximum(100);
    _laplacianSlider->setValue(25);
    connect(_laplacianSlider, &QSlider::valueChanged, this, &EnhancementWidget::laplacianValueChanged);
    _laplacianValueLabel = new QLabel("0.5");
    _laplacianValueLabel->setFixedWidth(25);

    QHBoxLayout* h2Layout = new QHBoxLayout;
    h2Layout->addWidget(new QLabel(tr("Sharpness:")));
    h2Layout->addWidget(_laplacianSlider);
    h2Layout->addWidget(_laplacianValueLabel);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(_sobelCheckBox);
    vLayout->addLayout(h1Layout);
    vLayout->addWidget(_laplacianCheckBox);
    vLayout->addLayout(h2Layout);

    groupBox->setLayout(vLayout);
    
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(groupBox);
    setLayout(layout);
}

void EnhancementWidget::sobelCheckBoxClicked()
{
    if (_sobelCheckBox->isChecked())
    {
        emit sobelChanged(_sobelFactor);
    }
    else
    {
        emit sobelChanged(0);
    }
}

void EnhancementWidget::laplacianCheckBoxClicked()
{
    if (_laplacianCheckBox->isChecked())
    {
        emit laplacianChanged(_laplacianFactor);
    }
    else
    {
        emit laplacianChanged(0);
    }
}

void EnhancementWidget::sobelValueChanged(int value)
{
    _sobelFactor = value / 100.0f;
    _sobelValueLabel->setText(QString::number(_sobelFactor));

    emit sobelChanged(_sobelFactor);
}

void EnhancementWidget::laplacianValueChanged(int value)
{
    _laplacianFactor = value / 50.0f;
    _laplacianValueLabel->setText(QString::number(_laplacianFactor));

    emit laplacianChanged(_laplacianFactor);
}

EnhancementProcessor::EnhancementProcessor(QObject* parent)
    : BaseProcessor(true, parent)
    , _sobelFactor(0.2f)
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
    connect(widget, &EnhancementWidget::sobelChanged, this, &EnhancementProcessor::sobelChanged);
    connect(widget, &EnhancementWidget::laplacianChanged, this, &EnhancementProcessor::laplacianChanged);
    emit createWidget(widget);
}

void EnhancementProcessor::sobelChanged(float value)
{
    _sobelFactor = value;

    processForView(getGlobalImage());
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
                float value = srcPixel[n];
                if (_sobelFactor > 0)
                {
                    float sobelValue = abs(srcPixel[n - pitch - depth] + 2 * srcPixel[n - pitch] + srcPixel[n - pitch + depth]
                        - srcPixel[n + pitch - depth] - 2 * srcPixel[n + pitch] - srcPixel[n + pitch + depth])
                        + abs(srcPixel[n - pitch - depth] + 2 * srcPixel[n - depth] + srcPixel[n + pitch - depth]
                            - srcPixel[n - pitch + depth] - 2 * srcPixel[n + depth] - srcPixel[n + pitch + depth]);
                    value += _sobelFactor * sobelValue;
                }

                if (_laplacianFactor > 0)
                {
                    float laplacianValue = srcPixel[n - pitch - depth] + srcPixel[n - pitch] + srcPixel[n - pitch + depth]
                        + srcPixel[n - depth] + srcPixel[n + depth]
                        + srcPixel[n + pitch - depth] + srcPixel[n + pitch] + srcPixel[n + pitch + depth]
                        - 8 * srcPixel[n];
                    value -= _laplacianFactor * laplacianValue;
                }

                if (value > 255)
                {
                    dstPixel[n] = 255;
                }
                else if (value < 0)
                {
                    dstPixel[n] = 0;
                }
                else
                {
                    dstPixel[n] = value;
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
            float value = srcImage->getValue(n);

            if (_sobelFactor > 0)
            {
                float sobelValue = abs(srcImage->getValue(n - width - 1) + 2 * srcImage->getValue(n - width) + srcImage->getValue(n - width + 1)
                    - srcImage->getValue(n + width - 1) - 2 * srcImage->getValue(n + width) - srcImage->getValue(n + width + 1))
                    + abs(srcImage->getValue(n - width - 1) + 2 * srcImage->getValue(n - 1) + srcImage->getValue(n + width - 1)
                        - srcImage->getValue(n - width + 1) - 2 * srcImage->getValue(n + 1) - srcImage->getValue(n + width + 1));
                value += _sobelFactor * sobelValue;
            }

            if (_laplacianFactor > 0)
            {
                float laplacianValue = srcImage->getValue(n - width - 1) + srcImage->getValue(n - width) + srcImage->getValue(n - width + 1)
                    + srcImage->getValue(n - 1) + srcImage->getValue(n + 1)
                    + srcImage->getValue(n + width - 1) + srcImage->getValue(n + width) + srcImage->getValue(n + width + 1)
                    - 8 * srcImage->getValue(n);
                value -= _laplacianFactor * laplacianValue;
            }

            if (value > maxValue)
            {
                dstImage->setValue(n, maxValue);
                dstByteImage[3 * n] = dstByteImage[3 * n + 1] = dstByteImage[3 * n + 2] = 255;
            }
            else if (value < minValue)
            {
                dstImage->setValue(n, minValue);
                dstByteImage[3 * n] = dstByteImage[3 * n + 1] = dstByteImage[3 * n + 2] = 0;
            }
            else
            {
                dstImage->setValue(n, value);
                dstByteImage[3 * n] = dstByteImage[3 * n + 1] = dstByteImage[3 * n + 2] = uchar((value - minValue) * variable);
            }
        }
    }

    dstImage->copyByteToImage();
}
