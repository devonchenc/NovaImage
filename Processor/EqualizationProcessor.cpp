#include "EqualizationProcessor.h"

#include <QLabel>
#include <QSlider>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QIntValidator>

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
#include "../Core/GlobalFunc.h"
#include "../Widget/CustomEvent.h"
#include "CLAHE.h"

EqualizationWidget::EqualizationWidget(BaseProcessor* processor, QWidget* parent)
    : ProcessorBaseWidget(processor, parent)
{
    QGroupBox* groupBox = new QGroupBox(tr("Equalization"));

    _gridSizeEdit = new QLineEdit("8");
    _gridSizeEdit->setValidator(new QIntValidator(2, 16, this));
    _gridSizeEdit->setToolTip(tr("The Image size should be divisible by grid size, \nand the maximum grid size is 16."));
    connect(_gridSizeEdit, &QLineEdit::editingFinished, this, &EqualizationWidget::editChanged);
    _clipLimitEdit = new QLineEdit("4");
    _clipLimitEdit->setValidator(new QIntValidator(1, 100, this));
    connect(_clipLimitEdit, &QLineEdit::editingFinished, this, &EqualizationWidget::editChanged);

    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel(tr("Tiles Grid Size:")), 0, 0);
    gridLayout->addWidget(_gridSizeEdit, 0, 1);
    gridLayout->addWidget(new QLabel(tr("Clip Limit:")), 1, 0);
    gridLayout->addWidget(_clipLimitEdit, 1, 1);

    groupBox->setLayout(gridLayout);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(groupBox);
    setLayout(vLayout);
}

void EqualizationWidget::editChanged()
{
    int gridSize = _gridSizeEdit->text().toInt();
    int clipLimit = _clipLimitEdit->text().toInt();

    emit valueChanged(gridSize, clipLimit);
}

EqualizationProcessor::EqualizationProcessor(QObject* parent)
    : BaseProcessor(true, parent)
    , _gridSize(8)
    , _clipLimit(4)
{
}

void EqualizationProcessor::initUI()
{
    _widget = new EqualizationWidget(this);
    connect(_widget, &EqualizationWidget::valueChanged, this, &EqualizationProcessor::valueChanged);
    emit createWidget(_widget);
}

void EqualizationProcessor::valueChanged(int gridSize, int clipLimit)
{
    _gridSize = gridSize;
    _clipLimit = clipLimit;

    processForView(getGlobalImage());
}

void EqualizationProcessor::processImage(const GeneralImage* srcImage, GeneralImage* dstImage)
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

    // Make sure newWidth and newHeight is divisible by _gridSize
    int newWidth = width / _gridSize * _gridSize;
    int newHeight = height / _gridSize * _gridSize;

    int* H = new int[newWidth * newHeight];
    float* S = new float[newWidth * newHeight];
    uchar* V = new uchar[newWidth * newHeight];
    for (int j = 0; j < newHeight; j++)
    {
        for (int i = 0; i < newWidth; i++)
        {
            int index = j * pitch + i * depth;
            if (depth < 3)
            {
                RGB2HSV(srcData[index], srcData[index], srcData[index],
                    H[j * newWidth + i], S[j * newWidth + i], V[j * newWidth + i]);
            }
            else
            {
                RGB2HSV(srcData[index + 2], srcData[index + 1], srcData[index],
                    H[j * newWidth + i], S[j * newWidth + i], V[j * newWidth + i]);
            }
        }
    }

    CLAHE(V, newWidth, newHeight, 0, 255, _gridSize, _gridSize, 256, _clipLimit);

    for (int j = 0; j < newHeight; j++)
    {
        for (int i = 0; i < newWidth; i++)
        {
            int index = j * pitch + i * depth;
            if (depth < 3)
            {
                HSV2RGB(H[j * newWidth + i], S[j * newWidth + i], V[j * newWidth + i],
                    dstData[index], dstData[index], dstData[index]);
            }
            else
            {
                HSV2RGB(H[j * newWidth + i], S[j * newWidth + i], V[j * newWidth + i],
                    dstData[index + 2], dstData[index + 1], dstData[index]);
            }
        }
    }

    delete[] S;
    delete[] V;
    delete[] H;
}

void EqualizationProcessor::processImage(const MonoImage* srcImage, MonoImage* dstImage)
{
    assert(srcImage);
    assert(dstImage);

    int width, height;
    const uchar* srcByteImage = srcImage->getBYTEImage(width, height);
    uchar* dstByteImage = dstImage->getBYTEImage(width, height);

    // Make sure newWidth and newHeight is divisible by _gridSize
    int newWidth = width / _gridSize * _gridSize;
    int newHeight = height / _gridSize * _gridSize;

    float maxValue = srcImage->getMaxValue();
    float minValue = srcImage->getMinValue();

    uchar* temp = new uchar[newWidth * newHeight];
    for (int j = 0; j < newHeight; j++)
    {
        for (int i = 0; i < newWidth; i++)
        {
            int index = j * width + i;
            temp[index] = srcByteImage[3 * index];
        }
    }

    CLAHE(temp, newWidth, newHeight, 0, 255, _gridSize, _gridSize, 256, _clipLimit);

    float variable = (maxValue - minValue) / 255.0f;
    for (int j = 0; j < newHeight; j++)
    {
        for (int i = 0; i < newWidth; i++)
        {
            int index = j * width + i;
            dstByteImage[3 * index] = dstByteImage[3 * index + 1] = dstByteImage[3 * index + 2] = temp[index];
            float dstValue = temp[index] * variable + minValue;
            dstImage->setValue(index, dstValue);
        }
    }

    delete[] temp;

    dstImage->copyByteToImage();
}

void EqualizationProcessor::RGB2HSV(uchar R, uchar G, uchar B, int& H, float& S, uchar& V)
{
    uchar C_max = qMax(qMax(R, G), qMax(G, B));
    uchar C_min = qMin(qMin(R, G), qMin(G, B));
    float delta = C_max - C_min;

    if (delta == 0)
    {
        H = 0;
    }
    else if (C_max == R)
    {
        if (G >= B)
        {
            H = int(60.0f * (G - B) / delta);
        }
        else
        {
            H = int(60.0f * (G - B) / delta + 360);
        }
    }
    else if (C_max == G)
    {
        H = int(60.0f * (B - R) / delta + 120);
    }
    else if (C_max == B)
    {
        H = int(60.0f * (R - G) / delta + 240);
    }

    if (C_max == 0)
    {
        S = 0;
    }
    else
    {
        S = delta / C_max;
    }

    V = C_max;
}

void EqualizationProcessor::HSV2RGB(int H, float S, uchar V, uchar& R, uchar& G, uchar& B)
{
    int h = H / 60 % 6;
    float f = float(H) / 60.0f - h;
    uchar p = V * (1 - S);
    uchar q = V * (1 - f * S);
    uchar t = V * (1 - (1 - f) * S);
    switch (h)
    {
    case 0:
        R = V;
        G = t;
        B = p;
        break;
    case 1:
        R = q;
        G = V;
        B = p;
        break;
    case 2:
        R = p;
        G = V;
        B = t;
        break;
    case 3:
        R = p;
        G = q;
        B = V;
        break;
    case 4:
        R = t;
        G = p;
        B = V;
        break;
    case 5:
        R = V;
        G = p;
        B = q;
        break;
    }
}
