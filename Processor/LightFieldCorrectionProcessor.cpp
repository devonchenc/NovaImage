#include "LightFieldCorrectionProcessor.h"

#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <cmath>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
#include "../Core/GlobalFunc.h"

LightFieldCorrectionWidget::LightFieldCorrectionWidget(BaseProcessor* processor, QWidget* parent)
    : ProcessorBaseWidget(processor, parent)
{
    QGroupBox* groupBox = new QGroupBox(tr("Light Field Correction"));

    QRadioButton* autoButton = new QRadioButton(tr("Automatic Correction"));
    autoButton->setChecked(true);
    QRadioButton* fileButton = new QRadioButton(tr("Select Light Field File"));
    connect(autoButton, &QRadioButton::clicked, this, &LightFieldCorrectionWidget::autoButtonToggled);
    connect(fileButton, &QRadioButton::clicked, this, &LightFieldCorrectionWidget::fileButtonToggled);
    _radioGroup = new QButtonGroup(this);
    _radioGroup->addButton(autoButton, 0);
    _radioGroup->addButton(fileButton, 1);

    _pathEdit = new QLineEdit("");
    QPushButton* browseButton = new QPushButton("...");
    browseButton->setMaximumWidth(30);
    connect(browseButton, &QPushButton::clicked, this, &LightFieldCorrectionWidget::browseButtonClicked);

    QHBoxLayout* h1Layout = new QHBoxLayout;
    h1Layout->addWidget(new QLabel(tr("File save path:")));
    h1Layout->addWidget(_pathEdit);
    h1Layout->addWidget(browseButton);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(autoButton);
    vLayout->addWidget(fileButton);
    vLayout->addLayout(h1Layout);

    groupBox->setLayout(vLayout);
    
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(groupBox);
    setLayout(layout);
}

void LightFieldCorrectionWidget::autoButtonToggled()
{
    if (_radioGroup->button(0)->isChecked())
    {
        emit typeChanged(0, _fileName);
    }
}

void LightFieldCorrectionWidget::fileButtonToggled()
{
    if (_radioGroup->button(1)->isChecked())
    {
        emit typeChanged(1, _fileName);
    }
}

void LightFieldCorrectionWidget::browseButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select light field file"), "/", tr("Scan file (*.ndr *.dr *.dcm)"));
    if (!fileName.isEmpty())
    {
        _pathEdit->setText(fileName);
        _fileName = fileName;
        _radioGroup->button(0)->setChecked(false);
        _radioGroup->button(1)->setChecked(true);
    }
}

LightFieldCorrectionProcessor::LightFieldCorrectionProcessor(QObject* parent)
    : BaseProcessor(true, parent)
    , _type(0)
{

}

void LightFieldCorrectionProcessor::initUI()
{
    LightFieldCorrectionWidget* widget = new LightFieldCorrectionWidget(this);
    _processorWidget = widget;
    connect(widget, &LightFieldCorrectionWidget::typeChanged, this, &LightFieldCorrectionProcessor::typeChanged);
    emit createWidget(widget);
}

void LightFieldCorrectionProcessor::typeChanged(int type, QString fileName)
{
    _type = type;
    _fileName = fileName;

    processForView(getGlobalImage());
}

void LightFieldCorrectionProcessor::processImage(const GeneralImage* srcImage, GeneralImage* dstImage)
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

    if (_type == 0)
    {
        // Set new image width as pow of 2
        int newWidth = int(pow(2, ceil(log2(width))));
        int newHeight = int(pow(2, ceil(log2(height))));

        // Variance
        float sigma = width / 16;
        fftw_complex* kernelOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * newWidth * newHeight);
        generateKernel(kernelOut, newWidth, newHeight, sigma);

        // FFT image
        fftw_complex* imageIn = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * newWidth * newHeight);
        fftw_complex* imageOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * newWidth * newHeight);
        memset(imageIn, 0, sizeof(fftw_complex) * newWidth * newHeight);
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                imageIn[j * newWidth + i][0] = srcData[j * pitch + i * depth];
                imageIn[j * newWidth + i][1] = 0;
            }
        }

        fftw_plan pf = fftw_plan_dft_2d(newWidth, newHeight, imageIn, imageOut, FFTW_FORWARD, FFTW_ESTIMATE);
        fftw_execute(pf);
        fftw_destroy_plan(pf);

        // Convolution
        for (int i = 0; i < newWidth * newHeight; i++)
        {
            double a = imageOut[i][0] * kernelOut[i][0] - imageOut[i][1] * kernelOut[i][1];
            double b = imageOut[i][0] * kernelOut[i][1] + imageOut[i][1] * kernelOut[i][0];
            imageOut[i][0] = a;
            imageOut[i][1] = b;
        }

        fftw_complex* imageOut2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * newWidth * newHeight);
        pf = fftw_plan_dft_2d(newWidth, newHeight, imageOut, imageOut2, FFTW_BACKWARD, FFTW_ESTIMATE);
        fftw_execute(pf);
        fftw_destroy_plan(pf);

        // Shift
        for (int j = 0; j < newHeight; j++)
        {
            for (int i = 0; i < newWidth; i++)
            {
                int x = (i + newWidth / 2) % newWidth;
                int y = (j + newHeight / 2) % newHeight;
                imageOut[y * width + x][0] = imageOut2[j * width + i][0];
                imageOut[y * width + x][1] = imageOut2[j * width + i][1];
            }
        }

        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                const uchar* srcPixel = srcData + j * pitch + i * depth;
                uchar* dstPixel = dstData + j * pitch + i * depth;
                float value = imageOut[j * newWidth + i][0] / (newWidth * newHeight);
                value = srcPixel[0] * 255.0f / value;
                for (int n = 0; n < qMin(depth, 3); n++)
                {
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
                        dstPixel[n] = uchar(value);
                    }
                }
            }
        }
    }
    else
    {
        std::shared_ptr<BaseImage> lightImage = std::make_shared<GeneralImage>(_fileName);
        if (lightImage.get())
        {
            if (lightImage->isOpenSucceed() == false)
                return;

            QImage* lightImageEntity = lightImage->getImageEntity();
            if (lightImage->width() != width || lightImage->height() != height || lightImageEntity->depth() /8 != depth)
            {
                QMessageBox::critical(nullptr, tr("Light field file error"),
                    tr("The light field image size does not match the size of opened file!"), QMessageBox::Ok);
                return;
            }

            uchar* lightData = lightImageEntity->bits();
            for (int j = 0; j < height; j++)
            {
                for (int i = 0; i < width; i++)
                {
                    const uchar* srcPixel = srcData + j * pitch + i * depth;
                    uchar* dstPixel = dstData + j * pitch + i * depth;
                    uchar* lightPixel = lightData + j * pitch + i * depth;
                    for (int n = 0; n < qMin(depth, 3); n++)
                    {
                        float value = srcPixel[n] * 255.0f / lightPixel[n];
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
                            dstPixel[n] = uchar(value);
                        }
                    }
                }
            }
        }
    }
}

void LightFieldCorrectionProcessor::processImage(const MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
    uchar* dstByteImage = dstImage->getBYTEImage(width, height);

    if (_type == 0)
    {
        // Set new image width as pow of 2
        int newWidth = int(pow(2, ceil(log2(width))));
        int newHeight = int(pow(2, ceil(log2(height))));

        // Variance
        float sigma = width / 16;
        fftw_complex* kernelOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * newWidth * newHeight);
        generateKernel(kernelOut, newWidth, newHeight, sigma);

        // FFT image
        fftw_complex* imageIn = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * newWidth * newHeight);
        fftw_complex* imageOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * newWidth * newHeight);
        memset(imageIn, 0, sizeof(fftw_complex) * newWidth * newHeight);
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                imageIn[j * newWidth + i][0] = srcImage->getValue(j * width + i);
                imageIn[j * newWidth + i][1] = 0;
            }
        }

        fftw_plan pf = fftw_plan_dft_2d(newWidth, newHeight, imageIn, imageOut, FFTW_FORWARD, FFTW_ESTIMATE);
        fftw_execute(pf);
        fftw_destroy_plan(pf);

        // Convolution
        for (int i = 0; i < newWidth * newHeight; i++)
        {
            double a = imageOut[i][0] * kernelOut[i][0] - imageOut[i][1] * kernelOut[i][1];
            double b = imageOut[i][0] * kernelOut[i][1] + imageOut[i][1] * kernelOut[i][0];
            imageOut[i][0] = a;
            imageOut[i][1] = b;
        }

        fftw_complex* imageOut2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * newWidth * newHeight);
        pf = fftw_plan_dft_2d(newWidth, newHeight, imageOut, imageOut2, FFTW_BACKWARD, FFTW_ESTIMATE);
        fftw_execute(pf);
        fftw_destroy_plan(pf);

        // Shift
        for (int j = 0; j < newHeight; j++)
        {
            for (int i = 0; i < newWidth; i++)
            {
                int x = (i + newWidth / 2) % newWidth;
                int y = (j + newHeight / 2) % newHeight;
                imageOut[y * width + x][0] = imageOut2[j * width + i][0];
                imageOut[y * width + x][1] = imageOut2[j * width + i][1];
            }
        }

        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                int n = j * width + i;
                float value = srcImage->getValue(n) / (imageOut[j * newWidth + i][0] / (newWidth * newHeight));
                dstImage->setValue(n, value);
            }
        }

        fftw_free(kernelOut);
        fftw_free(imageIn);
        fftw_free(imageOut);
        fftw_free(imageOut2);

        float newMax = dstImage->getValue(0);
        float newMin = dstImage->getValue(0);
        for (int i = 0; i < width * height; i++)
        {
            float value = dstImage->getValue(i);
            if (newMax < value)
            {
                newMax = value;
            }
            if (newMin > value)
            {
                newMin = value;
            }
        }
        float variable;
        if (newMax != newMin)
        {
            variable = 255.0f / (newMax - newMin);
        }
        else
        {
            variable = 0.0f;
        }
        for (int i = 0; i < width * height; i++)
        {
            float value = dstImage->getValue(i);
            dstByteImage[3 * i] = dstByteImage[3 * i + 1] = dstByteImage[3 * i + 2] = uchar((value - newMin) * variable);
        }
    }
    else
    {
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                int n = j * width + i;
                float value = srcImage->getValue(n);
                dstImage->setValue(n, value);
            }
        }
    }

    dstImage->copyByteToImage();
}

void LightFieldCorrectionProcessor::generateKernel(fftw_complex* kernel, int kernelWidth, int kernelHeight, float sigma)
{
    float* kernelArray = new float[kernelWidth * kernelHeight];
    getGaussianArray(kernelArray, kernelWidth, kernelHeight, sigma);

    // Kernel
    fftw_complex* kernelIn = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * kernelWidth * kernelHeight);
    for (int i = 0; i < kernelWidth * kernelHeight; i++)
    {
        kernelIn[i][0] = kernelArray[i];
        kernelIn[i][1] = 0;
    }
    fftw_plan pf = fftw_plan_dft_2d(kernelWidth, kernelHeight, kernelIn, kernel, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(pf);
    fftw_destroy_plan(pf);

    delete[] kernelArray;
    fftw_free(kernelIn);
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
