#include "ThresholdSegmentationProcessor.h"

#include <QHBoxLayout>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"

ThresholdSegmentationWidget::ThresholdSegmentationWidget(QWidget* parent)
    : QWidget(parent)
{
    _thresholdLabel = new QLabel(tr("Threshold:"));
    _thresholdSlider = new QSlider(Qt::Orientation::Horizontal);
    _thresholdSlider->setMinimum(0);
    _thresholdSlider->setMaximum(255);
    connect(_thresholdSlider, &QSlider::valueChanged, this, &ThresholdSegmentationWidget::valueChanged);
    _thresholdValueLabel = new QLabel;
    _thresholdValueLabel->setFixedWidth(20);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(_thresholdLabel);
    hLayout->addWidget(_thresholdSlider);
    hLayout->addWidget(_thresholdValueLabel);
    
    setLayout(hLayout);
}

void ThresholdSegmentationWidget::setThreshold(int threshold)
{
    _thresholdSlider->setValue(threshold);
}

void ThresholdSegmentationWidget::valueChanged(int value)
{
    _thresholdValueLabel->setText(QString::number(value));

 //   setThreshold();
}

ThresholdSegmentationProcessor::ThresholdSegmentationProcessor()
{

}

ThresholdSegmentationProcessor::~ThresholdSegmentationProcessor()
{

}

QWidget* ThresholdSegmentationProcessor::initUI()
{
    _widget = new ThresholdSegmentationWidget;
    emit createWidget(_widget);

    if (_image)
    {
        int width = _image->width();
        int height = _image->height();
        if (dynamic_cast<MonoImage*>(_image))
        {
            MonoImage* monoImage = dynamic_cast<MonoImage*>(_image);
            monoImage->getBYTEImage(width, height);
        }
        int threshold = findOtsuThreshold(_image->getGrayPixelArray(), width * height);
        _widget->setThreshold(threshold);
    }

    return _widget;
}

void ThresholdSegmentationProcessor::processGeneralImage(GeneralImage* image)
{
    assert(image);

    int width = image->width();
    int height = image->height();
    QImage* imageEntity = image->getImageEntity();
    uchar* pImageData = (uchar*)imageEntity->bits();
    int pitch = imageEntity->bytesPerLine();
    int depth = imageEntity->depth() / 8;

    int threshold = findOtsuThreshold(image->getGrayPixelArray(), width * height);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            uchar* pPixel = pImageData + j * pitch + i * depth;
            if (*pPixel > threshold)
            {
                for (int n = 0; n < qMin(depth, 3); n++)
                {
                    *(pPixel + n) = 255;
                }
            }
            else
            {
                for (int n = 0; n < qMin(depth, 3); n++)
                {
                    *(pPixel + n) = 0;
                }
            }
        }
    }
}

void ThresholdSegmentationProcessor::processMonoImage(MonoImage* image)
{
    assert(image);

    int width, height;
    uchar* byteImage = image->getBYTEImage(width, height);

    int threshold = findOtsuThreshold(image->getGrayPixelArray(), width * height);

    for (int i = 0; i < width * height; i++)
    {
        if (byteImage[3 * i] > threshold)
        {
            byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 255;
        }
        else
        {
            byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 0;
        }
    }

    image->copyByteToImage();
}

// Process float array
void ThresholdSegmentationProcessor::processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(minValue);
    Q_UNUSED(maxValue);
    assert(array && pByte);

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
        float var = (sumValue[grayscale - 1] * sumP[i] - sumValue[i]) * (sumValue[grayscale - 1] * sumP[i] - sumValue[i]) / (sumP[i] * (1 - sumP[i]));
        if (var > maxVar)
        {
            maxVar = var;
            threshold = i;
        }
    }

    delete[] pixelP;
    delete[] sumP;
    delete[] sumValue;

    return threshold;
}
