#include "LightFieldCorrectionProcessor.h"

#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
#include "../Core/GlobalFunc.h"
#include "fftw3.h"

LightFieldCorrectionWidget::LightFieldCorrectionWidget(BaseProcessor* processor, QWidget* parent)
    : ProcessorBaseWidget(processor, parent)
    , _sobelFactor(0.2f)
    , _laplacianFactor(0.5f)
{
    QGroupBox* groupBox = new QGroupBox(tr("Image Enhancement"));

    _sobelCheckBox = new QCheckBox(tr("Sobel"));
    _sobelCheckBox->setChecked(true);
    connect(_sobelCheckBox, &QCheckBox::clicked, this, &LightFieldCorrectionWidget::sobelCheckBoxClicked);

    _sobelSlider = new QSlider(Qt::Orientation::Horizontal);
    _sobelSlider->setMinimum(0);
    _sobelSlider->setMaximum(100);
    _sobelSlider->setValue(20);
    connect(_sobelSlider, &QSlider::valueChanged, this, &LightFieldCorrectionWidget::sobelValueChanged);
    _sobelValueLabel = new QLabel("0.2");
    _sobelValueLabel->setFixedWidth(25);

    QHBoxLayout* h1Layout = new QHBoxLayout;
    h1Layout->addWidget(new QLabel(tr("Sharpness:")));
    h1Layout->addWidget(_sobelSlider);
    h1Layout->addWidget(_sobelValueLabel);

    _laplacianCheckBox = new QCheckBox(tr("Laplacian"));
    _laplacianCheckBox->setChecked(true);
    connect(_laplacianCheckBox, &QCheckBox::clicked, this, &LightFieldCorrectionWidget::laplacianCheckBoxClicked);

    _laplacianSlider = new QSlider(Qt::Orientation::Horizontal);
    _laplacianSlider->setMinimum(0);
    _laplacianSlider->setMaximum(100);
    _laplacianSlider->setValue(25);
    connect(_laplacianSlider, &QSlider::valueChanged, this, &LightFieldCorrectionWidget::laplacianValueChanged);
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

void LightFieldCorrectionWidget::sobelCheckBoxClicked()
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

void LightFieldCorrectionWidget::laplacianCheckBoxClicked()
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

void LightFieldCorrectionWidget::sobelValueChanged(int value)
{
    _sobelFactor = value / 100.0f;
    _sobelValueLabel->setText(QString::number(_sobelFactor));

    emit sobelChanged(_sobelFactor);
}

void LightFieldCorrectionWidget::laplacianValueChanged(int value)
{
    _laplacianFactor = value / 50.0f;
    _laplacianValueLabel->setText(QString::number(_laplacianFactor));

    emit laplacianChanged(_laplacianFactor);
}

LightFieldCorrectionProcessor::LightFieldCorrectionProcessor(QObject* parent)
    : BaseProcessor(true, parent)
    , _sobelFactor(0.2f)
    , _laplacianFactor(0.5f)
{

}

LightFieldCorrectionProcessor::~LightFieldCorrectionProcessor()
{

}

void LightFieldCorrectionProcessor::initUI()
{
    LightFieldCorrectionWidget* widget = new LightFieldCorrectionWidget(this);
    _processorWidget = widget;
    connect(widget, &LightFieldCorrectionWidget::sobelChanged, this, &LightFieldCorrectionProcessor::sobelChanged);
    connect(widget, &LightFieldCorrectionWidget::laplacianChanged, this, &LightFieldCorrectionProcessor::laplacianChanged);
    emit createWidget(widget);
}

void LightFieldCorrectionProcessor::sobelChanged(float value)
{
    _sobelFactor = value;

    processForView(getGlobalImage());
}

void LightFieldCorrectionProcessor::laplacianChanged(float value)
{
    _laplacianFactor = value;

    processForView(getGlobalImage());
}

void LightFieldCorrectionProcessor::processImage(GeneralImage* srcImage, GeneralImage* dstImage)
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

void LightFieldCorrectionProcessor::processImage(MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
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

    // Variance
    float sigma = width / 16;
    float* kernel = new float[width * height];
    getGaussianArray(kernel, width, height, sigma);

    // Kernel
    fftw_complex* kernelIn = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * width * height);
    fftw_complex* kernelOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * width * height);
    for (int i = 0; i < width * height; i++)
    {
        kernelIn[i][0] = kernel[i];
        kernelIn[i][1] = 0;
    }
    fftw_plan pf = fftw_plan_dft_2d(width, height, kernelIn, kernelOut, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(pf);
    fftw_destroy_plan(pf);

    delete[] kernel;

    // FFT
    fftw_complex* imageIn = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * width * height);
    fftw_complex* imageOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * width * height);
    memset(imageIn, 0, sizeof(fftw_complex) * width * height);
    for (int i = 0; i < width * height; i++)
    {
        imageIn[i][0] = srcImage->getValue(i);
        imageIn[i][1] = 0;
    }

    pf = fftw_plan_dft_2d(width, height, imageIn, imageOut, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(pf);
    fftw_destroy_plan(pf);

    // Convolution
    for (int i = 0; i < width * height; i++)
    {
        double a = imageOut[i][0] * kernelOut[i][0] - imageOut[i][1] * kernelOut[i][1];
        double b = imageOut[i][0] * kernelOut[i][1] + imageOut[i][1] * kernelOut[i][0];
        imageOut[i][0] = a;
        imageOut[i][1] = b;
    }

    fftw_complex* imageOut2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * width * height);
    pf = fftw_plan_dft_2d(width, height, imageOut, imageOut2, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(pf);
    fftw_destroy_plan(pf);

    // Shift
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            int x = (i + width / 2) % width;
            int y = (j + height / 2) % height;
            imageOut[y * width + x][0] = imageOut2[j * width + i][0];
            imageOut[y * width + x][1] = imageOut2[j * width + i][1];
        }
    }

    for (int i = 0; i < width * height; i++)
    {
        float value = imageOut[i][0] / (width * height);
        dstImage->setValue(i, value);
        dstByteImage[3 * i] = dstByteImage[3 * i + 1] = dstByteImage[3 * i + 2] = uchar((value - minValue) * variable);
    }

    fftw_free(kernelIn);
    fftw_free(kernelOut);
    fftw_free(imageIn);
    fftw_free(imageOut);
    fftw_free(imageOut2);

    dstImage->copyByteToImage();
}

void LightFieldCorrectionProcessor::getGaussianArray(float* kernel, int width, int height, float sigma)
{
    int xCenter = width / 2;
    int yCenter = height / 2;
    float sum = 0.0f;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            kernel[j * width + i] = exp(-1.0f * ((i - xCenter) * (i - xCenter) + (j - yCenter) * (j - yCenter)) / (2.0f * sigma * sigma));
            sum += kernel[j * width + i];
        }
    }

    // Normalized
    for (int i = 0; i < width * height; i++)
    {
        kernel[i] /= sum;
    }
}