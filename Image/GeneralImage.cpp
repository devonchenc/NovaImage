﻿#include "GeneralImage.h"

#include <cmath>

GeneralImage::GeneralImage()
    : _backupImage(nullptr)
{
    _windowWidth = 255;
    _windowLevel = 127.5f;
}

GeneralImage::GeneralImage(const QString& pathName)
    : BaseImage(pathName)
    , _backupImage(nullptr)
{
    _pImage = std::make_shared<QImage>(pathName);
    _openSucceed = !_pImage->isNull();
    _width = _pImage->width();
    _height = _pImage->height();
    _windowWidth = 255;
    _windowLevel = 127.5f;

    // Backup origin QImage
    if (_openSucceed)
    {
        backupImage();
    }
}

GeneralImage::GeneralImage(const GeneralImage& src)
    : BaseImage(src)
    , _backupImage(src._backupImage)
{
}

GeneralImage& GeneralImage::operator=(const GeneralImage& src)
{
    if (this == &src)
        return *this;

    BaseImage::operator=(src);

    _backupImage = src._backupImage;

    return *this;
}

uchar GeneralImage::calcNewColor(uchar color, float bottom, float mid, float top, int minColor, int maxColor) const
{
    if (color <= bottom)
    {
        return minColor;
    }
    else if (color >= top)
    {
        return maxColor;
    }
    else
    {
        if (mid == 1)
        {
            return uchar((color - bottom)*(maxColor - minColor) / (top - bottom) + minColor);
        }
        else
        {
            int midColor = round(maxColor * mid / (1 + mid));
            if (color < (top + bottom) / 2)
            {
                return uchar((color - bottom)*(midColor - minColor) / ((top + bottom) / 2 - bottom) + minColor);
            }
            else
            {
                return uchar((color - (top + bottom) / 2)*(maxColor - midColor) / (top - (top + bottom) / 2) + midColor);
            }
        }
    }
}

// Histogram statistic
void GeneralImage::histogramStatistic()
{
    memset(_grayPixelNumber, 0, sizeof(uint) * 256);
    memset(_redPixelNumber, 0, sizeof(uint) * 256);
    memset(_greenPixelNumber, 0, sizeof(uint) * 256);
    memset(_bluePixelNumber, 0, sizeof(uint) * 256);

    for (int j = 0; j < _height; j++)
    {
        for (int i = 0; i < _width; i++)
        {
            QRgb value = getPixel(QPoint(i, j));
            _grayPixelNumber[(int)round(getValue(QPoint(i, j)))]++;
            _redPixelNumber[qRed(value)]++;
            _greenPixelNumber[qGreen(value)]++;
            _bluePixelNumber[qBlue(value)]++;
        }
    }

    // Clear the count numbers of pixels whose gray value less than 4
    memset(_grayPixelNumber, 0, sizeof(uint) * 3);
    memset(_redPixelNumber, 0, sizeof(uint) * 3);
    memset(_greenPixelNumber, 0, sizeof(uint) * 3);
    memset(_bluePixelNumber, 0, sizeof(uint) * 3);
}

float GeneralImage::getValue(const QPoint& position) const
{
    QRgb pixel = getPixel(position);
    return 0.299f * qRed(pixel) + 0.587f * qGreen(pixel) + 0.114f * qBlue(pixel);
}

float GeneralImage::getValue(int x, int y) const
{
    return getValue(QPoint(x, y));
}

float GeneralImage::getValue(int index) const
{
    QPoint point(index % _width, index / _width);
    QRgb pixel = getPixel(point);
    return 0.299f * qRed(pixel) + 0.587f * qGreen(pixel) + 0.114f * qBlue(pixel);
}

BaseImage* GeneralImage::copyImage() const
{
    return new GeneralImage(*this);
}

bool GeneralImage::copyToImage(BaseImage* image) const
{
    GeneralImage* destImage = dynamic_cast<GeneralImage*>(image);
    if (!destImage)
        return false;

    *destImage = *this;

    return true;
}

void GeneralImage::restore()
{
    *_pImage = *_backupImage;
}

// Backup origin QImage
void GeneralImage::backupImage()
{
    _backupImage = std::make_shared<QImage>(*_pImage);
}
