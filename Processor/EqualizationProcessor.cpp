#include "EqualizationProcessor.h"

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"
#include "CLAHE.h"

EqualizationProcessor::EqualizationProcessor()
{

}

EqualizationProcessor::~EqualizationProcessor()
{

}

void EqualizationProcessor::processGeneralImage(GeneralImage* image)
{
    assert(image);

    int width = image->width();
    int height = image->height();
    QImage* imageEntity = image->getImageEntity();
    uchar* pImageData = (uchar*)imageEntity->bits();
    int pitch = imageEntity->bytesPerLine();
    int depth = imageEntity->depth() / 8;

    int* H = new int[width * height];
    float* S = new float[width * height];
    uchar* V = new uchar[width * height];
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            int index = j * pitch + depth * i;
            RGB2HSV(pImageData[index + 2], pImageData[index + 1], pImageData[index],
                H[j * width + i], S[j * width + i], V[j * width + i]);
        }
    }

    CLAHE(V, width, height, 0, 255, 8, 8, 256, 8);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            int index = j * pitch + depth * i;
            HSV2RGB(H[j * width + i], S[j * width + i], V[j * width + i],
                pImageData[index + 2], pImageData[index + 1], pImageData[index]);
        }
    }

    delete[] S;
    delete[] V;
    delete[] H;
}

void EqualizationProcessor::processMonoImage(MonoImage* image)
{
    assert(image);

    int width, height;
    uchar* byteImage = image->getBYTEImage(width, height);

    uchar* temp = new uchar[width * height];
    for (int i = 0; i < width * height; i++)
    {
        temp[i] = byteImage[3 * i];
    }

    CLAHE(temp, width, height, 0, 255, 8, 8, 256, 8);

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