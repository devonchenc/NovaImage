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

    _thresholdSlider = new QSlider(Qt::Orientation::Horizontal);
    _thresholdSlider->setMinimum(0);
    _thresholdSlider->setMaximum(255);
    connect(_thresholdSlider, &QSlider::valueChanged, this, &EnhancementWidget::valueChanged);
    _thresholdValueLabel = new QLabel;
    _thresholdValueLabel->setFixedWidth(20);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(new QLabel(tr("Threshold:")));
    hLayout->addWidget(_thresholdSlider);
    hLayout->addWidget(_thresholdValueLabel);

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
    _thresholdSlider->setValue(threshold);
}

void EnhancementWidget::valueChanged(int value)
{
    _thresholdValueLabel->setText(QString::number(value));

    _autoCheckBox->setChecked(value == _OSTUThreshold);

    emit thresholdChanged(value);
}

void EnhancementWidget::autoCheckBoxClicked()
{
    if (_autoCheckBox->isChecked())
    {
        _thresholdSlider->setValue(_OSTUThreshold);
    }
}

EnhancementProcessor::EnhancementProcessor(QObject* parent)
    : BaseProcessor(true, parent)
    , _threshold(128)
{

}

EnhancementProcessor::~EnhancementProcessor()
{

}

void EnhancementProcessor::initUI()
{
    EnhancementWidget* widget = new EnhancementWidget(this);
    _processorWidget = widget;
    connect(widget, &EnhancementWidget::thresholdChanged, this, &EnhancementProcessor::thresholdChanged);
    emit createWidget(widget);

    if (_srcImage)
    {
        int width = _srcImage->width();
        int height = _srcImage->height();
    //    int threshold = findOtsuThreshold(_srcImage->getGrayPixelArray(), width * height);
//        widget->setOTSUThreshold(threshold);
    }
}

void EnhancementProcessor::thresholdChanged(int value)
{
    _threshold = value;

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

    int* temp = new int[(width - 2) * (height - 2) * depth];

    for (int j = 1; j < height - 1; j++)
    {
        for (int i = 1; i < width - 1; i++)
        {
            uchar* dstPixel = dstData + j * pitch + i * depth;
            uchar* srcPixel = srcData + j * pitch + i * depth;
            for (int n = 0; n < qMin(depth, 3); n++)
            {
                int laplacianValue = srcPixel[n - pitch - i * depth] + srcPixel[n - pitch] + srcPixel[n - pitch + i * depth]
                    + srcPixel[n - i * depth] + srcPixel[n + i * depth]
                    + srcPixel[n + pitch - i * depth] + srcPixel[n + pitch] + srcPixel[n + pitch + i * depth]
                    - 8 * srcPixel[n];
                temp[] = laplacianValue;
             //   dstPixel[n] = srcPixel[n] - 1 * laplacianValue;
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
