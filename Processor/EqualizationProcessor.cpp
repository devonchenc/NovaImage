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

EqualizationProcessor::~EqualizationProcessor()
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

    process();

    repaintView();
}

void EqualizationProcessor::processImageImpl(GeneralImage* image, QImage* dstImage)
{
    assert(image);

    int width = image->width();
    int height = image->height();
    QImage* imageEntity = image->getImageEntity();
    uchar* imageData = imageEntity->bits();
    uchar* dstData = dstImage->bits();
    int pitch = imageEntity->bytesPerLine();
    int depth = imageEntity->depth() / 8;
    int temp = imageEntity->depth();

    int* H = new int[width * height];
    float* S = new float[width * height];
    uchar* V = new uchar[width * height];
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            int index = j * pitch + i * depth;
            if (depth < 3)
            {
                RGB2HSV(imageData[index], imageData[index], imageData[index],
                    H[j * width + i], S[j * width + i], V[j * width + i]);
            }
            else
            {
                RGB2HSV(imageData[index + 2], imageData[index + 1], imageData[index],
                    H[j * width + i], S[j * width + i], V[j * width + i]);
            }
        }
    }

    CLAHE(V, width, height, 0, 255, _gridSize, _gridSize, 256, _clipLimit);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            int index = j * pitch + i * depth;
            if (depth < 3)
            {
                HSV2RGB(H[j * width + i], S[j * width + i], V[j * width + i],
                    dstData[index], dstData[index], dstData[index]);
            }
            else
            {
                HSV2RGB(H[j * width + i], S[j * width + i], V[j * width + i],
                    dstData[index + 2], dstData[index + 1], dstData[index]);
            }
        }
    }

    delete[] S;
    delete[] V;
    delete[] H;
}

void EqualizationProcessor::processImageImpl(MonoImage* image, QImage* dstImage)
{
    assert(image);

    int width, height;
    uchar* byteImage = image->getBYTEImage(width, height);

    MonoImage* backupMonoImage = dynamic_cast<MonoImage*>(_backupImage);
    backupMonoImage->setViewType(image->viewType());
    uchar* backupByteImage = backupMonoImage->getBYTEImage(width, height);

    uchar* temp = new uchar[width * height];
    for (int i = 0; i < width * height; i++)
    {
        temp[i] = backupByteImage[3 * i];
    }

    CLAHE(temp, width, height, 0, 255, _gridSize, _gridSize, 256, _clipLimit);

    for (int i = 0; i < width * height; i++)
    {
        byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = temp[i];
    }

    delete[] temp;

    image->copyByteToImage();
}

// Process float array
void EqualizationProcessor::processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(minValue);
    Q_UNUSED(maxValue);
    assert(array && pByte);

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